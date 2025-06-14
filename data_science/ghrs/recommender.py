import sys
import pandas as pd
import numpy as np
import networkx as nx
from sklearn.cluster import KMeans
from sklearn.preprocessing import LabelEncoder
import torch
import torch.nn as nn
from torch.utils.data import DataLoader, TensorDataset

# 데이터 로드 및 전처리 함수
def load_data(train_file, test_file, user_file, item_file):
    train_df = pd.read_csv(train_file, sep='\t', names=['user_id', 'item_id', 'rating', 'timestamp'])
    test_df = pd.read_csv(test_file, sep='\t', names=['user_id', 'item_id', 'rating', 'timestamp'])
    user_df = pd.read_csv(user_file, sep='|', names=['user_id', 'age', 'gender', 'occupation', 'zip'])
    item_df = pd.read_csv(item_file, sep='|', encoding='latin-1', 
                          names=['item_id', 'title', 'release_date', 'video_release', 'url'] + [f'genre_{i}' for i in range(19)])
    
    # 사용자 정보 인코딩
    le_gender = LabelEncoder()
    le_occupation = LabelEncoder()
    user_df['gender'] = le_gender.fit_transform(user_df['gender'])
    user_df['occupation'] = le_occupation.fit_transform(user_df['occupation'])
    
    return train_df, test_df, user_df, item_df

# 유사도 그래프 구축
def build_similarity_graph(train_df, alpha=0.01):
    user_item_matrix = train_df.pivot(index='user_id', columns='item_id', values='rating').fillna(0)
    G = nx.Graph()
    G.add_nodes_from(user_item_matrix.index)
    
    users = user_item_matrix.index
    for i in range(len(users)):
        for j in range(i + 1, len(users)):
            u1, u2 = users[i], users[j]
            ratings1 = user_item_matrix.loc[u1]
            ratings2 = user_item_matrix.loc[u2]
            common_items = ratings1[ratings1 > 0].index.intersection(ratings2[ratings2 > 0].index)
            if len(common_items) > 0:
                similar_count = np.sum(np.abs(ratings1[common_items] - ratings2[common_items]) <= 1)
                if similar_count / len(common_items) >= alpha:
                    G.add_edge(u1, u2)
    
    return G, user_item_matrix

# 그래프 기반 특징 추출
def extract_graph_features(G):
    features = {}
    features['pagerank'] = nx.pagerank(G)
    features['degree_centrality'] = nx.degree_centrality(G)
    features['closeness_centrality'] = nx.closeness_centrality(G)
    features['betweenness_centrality'] = nx.betweenness_centrality(G)
    features['load_centrality'] = nx.load_centrality(G)
    features['avg_neighbor_degree'] = nx.average_neighbor_degree(G)
    
    feature_df = pd.DataFrame(features).fillna(0)
    return feature_df

# 오토인코더 정의
class Autoencoder(nn.Module):
    def __init__(self, input_dim):
        super(Autoencoder, self).__init__()
        self.encoder = nn.Sequential(
            nn.Linear(input_dim, 64),
            nn.ReLU(),
            nn.Linear(64, 32),
            nn.ReLU(),
            nn.Linear(32, 4)  # 축소된 차원
        )
        self.decoder = nn.Sequential(
            nn.Linear(4, 32),
            nn.ReLU(),
            nn.Linear(32, 64),
            nn.ReLU(),
            nn.Linear(64, input_dim)
        )
    
    def forward(self, x):
        encoded = self.encoder(x)
        decoded = self.decoder(encoded)
        return decoded, encoded

# 오토인코더 학습
def train_autoencoder(features, epochs=100, batch_size=32):
    device = torch.device('cuda' if torch.cuda.is_available() else 'cpu')
    model = Autoencoder(features.shape[1]).to(device)
    optimizer = torch.optim.Adam(model.parameters(), lr=0.001)
    criterion = nn.MSELoss()
    
    dataset = TensorDataset(torch.FloatTensor(features.values))
    loader = DataLoader(dataset, batch_size=batch_size, shuffle=True)
    
    for _ in range(epochs):
        for data in loader:
            inputs = data[0].to(device)
            optimizer.zero_grad()
            outputs, _ = model(inputs)
            loss = criterion(outputs, inputs)
            loss.backward()
            optimizer.step()
    
    with torch.no_grad():
        encoded = model(torch.FloatTensor(features.values).to(device))[1]
    return encoded.cpu().numpy(), model

# 메인 함수
def main(train_file, test_file):
    # 데이터 로드
    train_df, test_df, user_df, item_df = load_data(train_file, test_file, 'u.user', 'u.item')
    
    # 유사도 그래프 구축
    G, user_item_matrix = build_similarity_graph(train_df)
    
    # 그래프 특징 추출
    graph_features = extract_graph_features(G)
    
    # 사용자 측 정보와 결합
    combined_features = pd.merge(graph_features, user_df[['user_id', 'age', 'gender', 'occupation']], 
                                 left_index=True, right_on='user_id', how='left').set_index('user_id')
    
    # 오토인코더로 차원 축소
    encoded_features, ae_model = train_autoencoder(combined_features)
    
    # K-means 클러스터링
    kmeans = KMeans(n_clusters=8, random_state=42)  # Elbow 방법으로 선택된 K=8
    clusters = kmeans.fit_predict(encoded_features)
    cluster_df = pd.DataFrame({'user_id': combined_features.index, 'cluster': clusters})
    
    # 클러스터별 평균 평점 계산
    train_with_cluster = pd.merge(train_df, cluster_df, on='user_id')
    cluster_ratings = train_with_cluster.groupby(['cluster', 'item_id'])['rating'].mean().reset_index()
    
    # 테스트 데이터 예측
    test_with_cluster = pd.merge(test_df, cluster_df, on='user_id', how='left')
    predictions = []
    for _, row in test_with_cluster.iterrows():
        user_id, item_id, cluster = row['user_id'], row['item_id'], row['cluster']
        if pd.isna(cluster):  # 새로운 사용자
            cluster = kmeans.predict(ae_model.encoder(torch.FloatTensor(
                user_df[user_df['user_id'] == user_id][['age', 'gender', 'occupation']].values)).detach().cpu().numpy())[0]
        cluster_rating = cluster_ratings[(cluster_ratings['cluster'] == cluster) & 
                                        (cluster_ratings['item_id'] == item_id)]['rating']
        if not cluster_rating.empty:
            pred_rating = cluster_rating.values[0]
        else:
            pred_rating = train_with_cluster[train_with_cluster['cluster'] == cluster]['rating'].mean() or 3.0
        predictions.append([user_id, item_id, round(pred_rating)])
    
    # 출력 파일 생성
    output_file = train_file.replace('.base', '_prediction.txt')
    pd.DataFrame(predictions, columns=['user_id', 'item_id', 'rating']).to_csv(
        output_file, sep='\t', index=False, header=False)

if __name__ == "__main__":
    if len(sys.argv) != 3:
        print("Usage: recommender.exe <train_file> <test_file>")
        sys.exit(1)
    main(sys.argv[1], sys.argv[2])