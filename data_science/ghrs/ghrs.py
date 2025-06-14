import sys
import pandas as pd
import numpy as np
import networkx as nx
from sklearn.preprocessing import OneHotEncoder, LabelEncoder, MinMaxScaler
from tensorflow.keras.models import Model
from tensorflow.keras.layers import Input, Dense
from sklearn.cluster import KMeans

def build_graph(train_df, alpha=0.01):
    G = nx.Graph()
    G.add_nodes_from(train_df['user_id'].unique())
    grouped = train_df.groupby('item_id')
    common = {}
    agree = {}
    for _, grp in grouped:
        users, rates = grp['user_id'].values, grp['rating'].values
        for i in range(len(users)):
            for j in range(i+1, len(users)):
                u, v = users[i], users[j]
                key = tuple(sorted((u,v)))
                common[key] = common.get(key,0) + 1
                if abs(rates[i] - rates[j]) <= 1:
                    agree[key] = agree.get(key,0) + 1
    for (u,v), c in common.items():
        if agree.get((u,v),0)/c >= alpha:
            G.add_edge(u, v)
    return G

def extract_graph_features(G):
    feats = {}
    feats['pagerank'] = nx.pagerank(G)
    feats['degree'] = nx.degree_centrality(G)
    feats['closeness'] = nx.closeness_centrality(G)
    feats['betweenness'] = nx.betweenness_centrality(G)
    feats['load'] = nx.load_centrality(G)
    feats['avg_neighbor'] = nx.average_neighbor_degree(G)
    df = pd.DataFrame(feats).fillna(0)
    df = pd.DataFrame(MinMaxScaler().fit_transform(df), index=df.index, columns=df.columns)
    return df

def encode_user_info(user_df):
    # gender, occupation 한-핫 인코딩 / age를 범주화 후 one-hot
    ohe = OneHotEncoder(sparse_output=False, handle_unknown='ignore')
    user_df['age_cat'] = pd.cut(user_df['age'], bins=[0,18,25,35,50,100], labels=False)
    enc = ohe.fit_transform(user_df[['gender','occupation','age_cat']])
    return pd.DataFrame(enc, index=user_df['user_id'])

def train_autoencoder(X, encoding_dim=8, epochs=50):
    from tensorflow.keras.optimizers import Adam
    inp = Input(shape=(X.shape[1],))
    h = Dense(32, activation='relu')(inp)
    code = Dense(encoding_dim, activation='relu')(h)
    dec = Dense(32, activation='relu')(code)
    out = Dense(X.shape[1], activation='sigmoid')(dec)
    ae = Model(inp, out)
    enc_model = Model(inp, code)
    ae.compile(optimizer=Adam(), loss='mse')
    ae.fit(X, X, batch_size=64, epochs=epochs, verbose=0)
    return enc_model.predict(X), enc_model

def main(train_file, test_file):
    train_df = pd.read_csv(train_file, sep='\t', names=['user_id','item_id','rating','timestamp'])
    test_df  = pd.read_csv(test_file,  sep='\t', names=['user_id','item_id','rating','timestamp'])
    user_df  = pd.read_csv('u.user', sep='|', names=['user_id','age','gender','occupation','zip'])
    G = build_graph(train_df, alpha=0.01)
    gfeat = extract_graph_features(G)
    ufeat = encode_user_info(user_df)
    F = pd.concat([gfeat, ufeat], axis=1).fillna(0)
    codes, enc_model = train_autoencoder(F.values, encoding_dim=8, epochs=100)
    clusters = KMeans(n_clusters=8, random_state=42).fit_predict(codes)
    cluster = pd.Series(clusters, index=F.index)

    # cluster-item 평균
    ci = train_df.copy().join(cluster.rename('cluster'), on='user_id')
    cmap = ci.groupby(['cluster','item_id'])['rating'].mean().unstack(fill_value=np.nan)

    out = test_file.replace('.test','.base_prediction.txt')
    with open(out, 'w') as fw:
        for _, r in test_df.iterrows():
            u, i = r['user_id'], r['item_id']
            c = cluster.get(u, np.nan)
            if pd.notna(c) and pd.notna(cmap.at[c,i]):
                pred = cmap.at[c,i]
            else:
                pred = ci[ci['cluster']==c]['rating'].mean() if pd.notna(c) else train_df['rating'].mean()
            fw.write(f"{u}\t{i}\t{pred:.2f}\n")

if __name__ == "__main__":
    if len(sys.argv)!=3:
        print("Usage: python ghrs_recommender.py u#.base u#.test")
        sys.exit(1)
    main(sys.argv[1], sys.argv[2])
