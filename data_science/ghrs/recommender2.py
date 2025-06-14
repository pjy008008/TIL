import sys
import pandas as pd
import numpy as np
import networkx as nx
from sklearn.cluster import KMeans
from sklearn.preprocessing import LabelEncoder
import torch
import torch.nn as nn
from torch.utils.data import DataLoader, TensorDataset
from tqdm import tqdm  # 진행 상황 표시를 위해 추가

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
    
    # tqdm으로 에포크 진행 상황 표시
    for epoch in tqdm(range(epochs), desc="Training Autoencoder"):
        epoch_loss = 0.0
        for data in loader:
            inputs = data[0].to(device)
            optimizer.zero_grad()
            outputs, _ = model(inputs)
            loss = criterion(outputs, inputs)
            loss.backward()
            optimizer.step()
            epoch_loss += loss.item() * inputs.size(0)
        # 에포크별 평균 손실 출력
        avg_loss = epoch_loss / len(dataset)
        print(f"Epoch {epoch + 1}/{epochs}, Average Loss: {avg_loss:.6f}")
    
    with torch.no_grad():
        encoded = model(torch.FloatTensor(features.values).to(device))[1]
    return encoded.cpu().numpy(), model

# 메인 함수
def main(train_file, test_file):
    print("Starting data loading...")
    train_df, test_df, user_df, item_df = load_data(train_file, test_file, 'u.user', 'u.item')
    print("Data loading completed.")
    
    print("Building similarity graph...")
    G, user_item_matrix = build_similarity_graph(train_df)
    print("Similarity graph built.")
    
    print("Extracting graph features...")
    graph_features = extract_graph_features(G)
    print("Graph features extracted.")
    
    print("Combining features...")
    combined_features = pd.merge(graph_features, user_df[['user_id', 'age', 'gender', 'occupation']], 
                                 left_index=True, right_on='user_id', how='left').set_index('user_id')
    print("Features combined.")
    
    print("Training autoencoder...")
    encoded_features, ae_model = train_autoencoder(combined_features)
    print("Autoencoder training completed.")
    
    print("Performing K-means clustering...")
    kmeans = KMeans(n_clusters=8, random_state=42)
    clusters = kmeans.fit_predict(encoded_features)
    cluster_df = pd.DataFrame({'user_id': combined_features.index, 'cluster': clusters})
    print("K-means clustering completed.")
    
    print("Calculating cluster ratings...")
    train_with_cluster = pd.merge(train_df, cluster_df, on='user_id')
    cluster_ratings = train_with_cluster.groupby(['cluster', 'item_id'])['rating'].mean().reset_index()
    print("Cluster ratings calculated.")
    
    print("Predicting ratings...")
    test_with_cluster = pd.merge(test_df, cluster_df, on='user_id', how='left')
    predictions = []
    for i, row in enumerate(test_with_cluster.iterrows()):
        if i % 1000 == 0:
            print(f"Processing test sample {i}/{len(test_with_cluster)}")
        user_id, item_id, cluster = row[1]['user_id'], row[1]['item_id'], row[1]['cluster']
        if pd.isna(cluster):
            user_features = user_df[user_df['user_id'] == user_id][['age', 'gender', 'occupation']].values
            if len(user_features) > 0:
                cluster = kmeans.predict(ae_model.encoder(torch.FloatTensor(user_features)).detach().cpu().numpy())[0]
            else:
                cluster = 0
        cluster_rating = cluster_ratings[(cluster_ratings['cluster'] == cluster) & 
                                        (cluster_ratings['item_id'] == item_id)]['rating']
        if not cluster_rating.empty:
            pred_rating = cluster_rating.values[0]
        else:
            pred_rating = train_with_cluster[train_with_cluster['cluster'] == cluster]['rating'].mean()
            if pd.isna(pred_rating):
                pred_rating = 3.0
        predictions.append([user_id, item_id, round(pred_rating)])
    print("Rating prediction completed.")
    
    print("Saving predictions...")
    output_file = train_file.replace('.base', '.base_prediction.txt')
    pd.DataFrame(predictions, columns=['user_id', 'item_id', 'rating']).to_csv(
        output_file, sep='\t', index=False, header=False)
    print(f"Predictions saved to {output_file}")

if __name__ == "__main__":
    if len(sys.argv) != 3:
        print("Usage: python recommender.py <train_file> <test_file>")
        sys.exit(1)
    main(sys.argv[1], sys.argv[2])