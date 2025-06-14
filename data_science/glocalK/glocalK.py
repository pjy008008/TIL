import sys
import pandas as pd
import numpy as np
import torch
import torch.nn as nn
import torch.nn.functional as F
from torch.utils.data import DataLoader, TensorDataset
from scipy.sparse import coo_matrix

# 2D-RBF 커널 함수 정의
def rbf_kernel(U, V):
    U = U.unsqueeze(1)  # (m, 1, d)
    V = V.unsqueeze(0)  # (1, n, d)
    diff = U - V
    squared_distance = torch.sum(diff ** 2, dim=2)
    return torch.clamp(1 - squared_distance, min=0)

# 로컬 커널화된 가중치 행렬을 사용한 오토인코더 정의
class LocalKernelAE(nn.Module):
    def __init__(self, num_users, hidden_dim):
        super(LocalKernelAE, self).__init__()
        self.num_users = num_users
        self.hidden_dim = hidden_dim
        # Weight matrices
        self.W_e = nn.Parameter(torch.randn(num_users, hidden_dim))  # (943, 500)
        self.W_d = nn.Parameter(torch.randn(hidden_dim, num_users))  # (500, 943)
        self.b_e = nn.Parameter(torch.zeros(hidden_dim))
        self.b_d = nn.Parameter(torch.zeros(num_users))
        # Embeddings for RBF kernel
        self.U_e = nn.Parameter(torch.randn(num_users, 5))    # 사용자 embedding
        self.V_e = nn.Parameter(torch.randn(hidden_dim, 5))   # 은닉층 embedding
        self.U_d = nn.Parameter(torch.randn(hidden_dim, 5))
        self.V_d = nn.Parameter(torch.randn(num_users, 5))
    
    def forward(self, x):
        # Compute kernelized weights
        K_e = rbf_kernel(self.U_e, self.V_e)  # (num_users, hidden_dim) = (943, 500)
        K_d = rbf_kernel(self.U_d, self.V_d)  # (hidden_dim, num_users) = (500, 943)
        W_e_prime = self.W_e * K_e
        W_d_prime = self.W_d * K_d
        # Encoder
        h = torch.relu(torch.matmul(x, W_e_prime) + self.b_e)
        # Decoder
        x_recon = torch.matmul(h, W_d_prime) + self.b_d
        return x_recon

# 글로벌 커널 컨볼루션 정의
def global_kernel_convolution(R, GK):
    print("Applying global kernel convolution...")
    R_tensor = torch.tensor(R, dtype=torch.float32).unsqueeze(0).unsqueeze(0)
    GK_tensor = torch.tensor(GK, dtype=torch.float32).unsqueeze(0).unsqueeze(0)
    R_conv = F.conv2d(R_tensor, GK_tensor, padding=1)
    print("Global kernel convolution completed.")
    return R_conv.squeeze().numpy()

# GLocal-K 모델 정의
class GLocalK(nn.Module):
    def __init__(self, num_users, hidden_dim):
        super(GLocalK, self).__init__()
        self.ae = LocalKernelAE(num_users, hidden_dim)
    
    def forward(self, x):
        return self.ae(x)

# 데이터 로딩 및 전처리
def load_data(train_file, test_file):
    print("Loading data...")
    train_df = pd.read_csv(train_file, sep='\t', names=['user_id', 'item_id', 'rating', 'timestamp'])
    test_df = pd.read_csv(test_file, sep='\t', names=['user_id', 'item_id', 'rating', 'timestamp'])
    print("Data loaded.")
    return train_df, test_df

# 사용자-아이템 행렬 생성
def create_user_item_matrix(df, num_users, num_items):
    print("Creating user-item matrix...")
    row = df['user_id'] - 1
    col = df['item_id'] - 1
    data = df['rating']
    matrix = coo_matrix((data, (row, col)), shape=(num_users, num_items)).toarray()
    print("User-item matrix created.")
    return matrix

# 로컬 커널로 사전 훈련
def pretrain_ae(model, R, epochs=30, batch_size=32, lr=0.001):
    print("Starting pre-training...")
    optimizer = torch.optim.Adam(model.parameters(), lr=lr)
    criterion = nn.MSELoss()
    dataset = TensorDataset(torch.tensor(R, dtype=torch.float32))
    loader = DataLoader(dataset, batch_size=batch_size, shuffle=True)
    
    for epoch in range(epochs):
        epoch_loss = 0.0
        for batch in loader:
            inputs = batch[0]
            optimizer.zero_grad()
            outputs = model(inputs)
            loss = criterion(outputs, inputs)
            loss.backward()
            optimizer.step()
            epoch_loss += loss.item() * inputs.size(0)
        avg_loss = epoch_loss / len(dataset)
        print(f"Pre-training Epoch {epoch+1}/{epochs}, Average Loss: {avg_loss:.6f}")
    print("Pre-training completed.")

# 글로벌 커널로 미세 조정
def finetune_ae(model, R_hat, epochs=20, batch_size=32, lr=0.001):
    print("Starting fine-tuning...")
    optimizer = torch.optim.Adam(model.parameters(), lr=lr)
    criterion = nn.MSELoss()
    dataset = TensorDataset(torch.tensor(R_hat, dtype=torch.float32))
    loader = DataLoader(dataset, batch_size=batch_size, shuffle=True)
    
    for epoch in range(epochs):
        epoch_loss = 0.0
        for batch in loader:
            inputs = batch[0]
            optimizer.zero_grad()
            outputs = model(inputs)
            loss = criterion(outputs, inputs)
            loss.backward()
            optimizer.step()
            epoch_loss += loss.item() * inputs.size(0)
        avg_loss = epoch_loss / len(dataset)
        print(f"Fine-tuning Epoch {epoch+1}/{epochs}, Average Loss: {avg_loss:.6f}")
    print("Fine-tuning completed.")

# 메인 함수
def main(train_file, test_file):
    # 데이터 로드
    train_df, test_df = load_data(train_file, test_file)
    
    # 사용자와 아이템 수 계산
    num_users = max(train_df['user_id'].max(), test_df['user_id'].max())
    num_items = max(train_df['item_id'].max(), test_df['item_id'].max())
    print(f"Number of users: {num_users}, Number of items: {num_items}")
    
    # 사용자-아이템 행렬 생성
    R = create_user_item_matrix(train_df, num_users, num_items)
    
    # GLocal-K 모델 초기화
    model = GLocalK(num_users, 500)
    
    # 오토인코더 사전 훈련
    pretrain_ae(model, R.T)  # 아이템 기반 (num_items × num_users)
    
    # 글로벌 커널 기반 평점 행렬 생성
    print("Generating global kernel-based rating matrix...")
    R_prime = model.ae(torch.tensor(R.T, dtype=torch.float32)).detach().numpy()
    mu = np.mean(R_prime, axis=1)  # Item-based average pooling
    K = np.random.randn(num_items, 3, 3)  # 3x3 kernels
    GK = np.sum([mu[i] * K[i] for i in range(num_items)], axis=0)
    R_hat = global_kernel_convolution(R, GK)
    print("Global kernel-based rating matrix generated.")
    
    # 오토인코더 미세 조정
    finetune_ae(model, R_hat.T)
    
    # 테스트 데이터에 대한 평점 예측
    print("Predicting ratings for test data...")
    # 전체 행렬을 모델에 통과시켜 예측 행렬 생성
    R_pred = model.ae(torch.tensor(R.T, dtype=torch.float32)).detach().numpy().T  # (num_users, num_items)
    test_user_ids = test_df['user_id'] - 1
    test_item_ids = test_df['item_id'] - 1
    predictions = []
    for user_id, item_id in zip(test_user_ids, test_item_ids):
        pred_rating = R_pred[user_id, item_id]
        pred_rating = max(1, min(5, round(pred_rating)))  # Clip to [1, 5]
        predictions.append([user_id + 1, item_id + 1, pred_rating])
    print("Rating prediction completed.")
    
    # 예측 결과 저장
    output_file = train_file.replace('.base', '_prediction.txt')
    pd.DataFrame(predictions, columns=['user_id', 'item_id', 'rating']).to_csv(
        output_file, sep='\t', index=False, header=False)
    print(f"Predictions saved to {output_file}")

if __name__ == "__main__":
    if len(sys.argv) != 3:
        print("Usage: python glocal_k_recommender.py <train_file> <test_file>")
        sys.exit(1)
    main(sys.argv[1], sys.argv[2])