import sys
import pandas as pd
import numpy as np
import networkx as nx
import itertools
from collections import Counter
from sklearn.cluster import KMeans
from sklearn.preprocessing import LabelEncoder
import torch
import torch.nn as nn
from torch.utils.data import DataLoader, TensorDataset

# 1) 데이터 로드 및 전처리
def load_data(train_file, test_file, user_file, item_file):
    train_df = pd.read_csv(train_file, sep='\t', names=['user_id','item_id','rating','timestamp'])
    test_df  = pd.read_csv(test_file,  sep='\t', names=['user_id','item_id','rating','timestamp'])
    user_df  = pd.read_csv(user_file, sep='|', names=['user_id','age','gender','occupation','zip'])
    item_df  = pd.read_csv(item_file, sep='|', encoding='latin-1',
                           names=['item_id','title','release_date','video_release','url'] + [f'genre_{i}' for i in range(19)])
    # 인코딩
    le_g  = LabelEncoder()
    le_o  = LabelEncoder()
    user_df['gender']     = le_g.fit_transform(user_df['gender'])
    user_df['occupation'] = le_o.fit_transform(user_df['occupation'])
    return train_df, test_df, user_df, item_df

# 2) 아이템 기준 co-rating으로 그래프 구축
def build_similarity_graph(train_df, alpha=0.01):
    # 사용자×아이템 매트릭스
    user_item = train_df.pivot(index='user_id', columns='item_id', values='rating').fillna(0)
    # (u,v) 공통 평점 및 유사 평점 개수 카운트
    common_cnt  = Counter()
    similar_cnt = Counter()
    # 아이템별로 평점 그룹
    for _, grp in train_df.groupby('item_id')[['user_id','rating']]:
        users   = grp['user_id'].values
        ratings = grp['rating'].values
        # 해당 아이템 평점한 사용자 쌍만
        for i in range(len(users)):
            for j in range(i+1, len(users)):
                u1, r1 = users[i], ratings[i]
                u2, r2 = users[j], ratings[j]
                key = (u1,u2) if u1 < u2 else (u2,u1)
                common_cnt[key] += 1
                if abs(r1 - r2) <= 1:
                    similar_cnt[key] += 1
    # 그래프 생성: 비율 ≥ alpha 인 엣지만
    G = nx.Graph()
    G.add_nodes_from(user_item.index)
    for key, com in common_cnt.items():
        if similar_cnt[key] / com >= alpha:
            G.add_edge(*key)
    return G, user_item

# 3) 그래프 기반 특징: Pagerank + Degree Centrality
def extract_graph_features(G):
    pr = nx.pagerank(G)                       # 빠르고 유의미
    dc = nx.degree_centrality(G)              # O(N+M)
    df = pd.DataFrame({'pagerank':pr, 'degree_centrality':dc}).fillna(0)
    return df

# 4) 오토인코더 정의
class Autoencoder(nn.Module):
    def __init__(self, input_dim):
        super().__init__()
        self.encoder = nn.Sequential(
            nn.Linear(input_dim, 64), nn.ReLU(),
            nn.Linear(64, 32),        nn.ReLU(),
            nn.Linear(32, 4)
        )
        self.decoder = nn.Sequential(
            nn.Linear(4, 32),  nn.ReLU(),
            nn.Linear(32, 64), nn.ReLU(),
            nn.Linear(64, input_dim)
        )
    def forward(self, x):
        code = self.encoder(x)
        recon = self.decoder(code)
        return recon, code

# 5) 오토인코더 학습
def train_autoencoder(features, epochs=100, batch_size=32):
    device = torch.device('cuda' if torch.cuda.is_available() else 'cpu')
    model  = Autoencoder(features.shape[1]).to(device)
    opt    = torch.optim.Adam(model.parameters(), lr=1e-3)
    crit   = nn.MSELoss()
    dataset= TensorDataset(torch.FloatTensor(features.values))
    loader = DataLoader(dataset, batch_size=batch_size, shuffle=True,
                        num_workers=4, pin_memory=True)
    for _ in range(epochs):
        for (x_batch,) in loader:
            x = x_batch.to(device)
            opt.zero_grad()
            recon, _ = model(x)
            loss = crit(recon, x)
            loss.backward()
            opt.step()
    with torch.no_grad():
        _, codes = model(torch.FloatTensor(features.values).to(device))
    return codes.cpu().numpy(), model

# 6) 메인
def main(train_file, test_file):
    train_df, test_df, user_df, item_df = load_data(train_file, test_file, 'u.user', 'u.item')
    G, user_item = build_similarity_graph(train_df, alpha=0.01)
    graph_feats   = extract_graph_features(G)
    combined = pd.merge(graph_feats, user_df[['user_id','age','gender','occupation']],
                        left_index=True, right_on='user_id', how='left').set_index('user_id')
    encoded, ae_model = train_autoencoder(combined)
    clusters = KMeans(n_clusters=8, random_state=42).fit_predict(encoded)
    cluster_df = pd.DataFrame({'user_id':combined.index, 'cluster':clusters})

    # 클러스터별 평균 평점
    trc = pd.merge(train_df, cluster_df, on='user_id')
    cluster_ratings = trc.groupby(['cluster','item_id'])['rating'].mean().reset_index()

    # 테스트 예측
    twc = pd.merge(test_df, cluster_df, on='user_id', how='left')
    preds = []
    for _, row in twc.iterrows():
        u,i,c = row['user_id'], row['item_id'], row['cluster']
        if pd.isna(c):
            uf = user_df[user_df['user_id']==u][['age','gender','occupation']].values
            if len(uf)>0:
                c = KMeans(n_clusters=8, random_state=42).fit_predict(ae_model.encoder(torch.FloatTensor(uf)).detach().cpu().numpy())[0]
            else:
                c = 0
        cr = cluster_ratings[(cluster_ratings['cluster']==c)&(cluster_ratings['item_id']==i)]['rating']
        if not cr.empty:
            r = cr.values[0]
        else:
            avg = trc[trc['cluster']==c]['rating'].mean()
            r = avg if not pd.isna(avg) else 3.0
        preds.append([u, i, round(r)])

    out = train_file.replace('.base','_prediction.txt')
    pd.DataFrame(preds, columns=['user_id','item_id','rating']).to_csv(out, sep='\t', index=False, header=False)

if __name__ == "__main__":
    if len(sys.argv)!=3:
        print("Usage: python recommender.py <train_file> <test_file>")
        sys.exit(1)
    main(sys.argv[1], sys.argv[2])
