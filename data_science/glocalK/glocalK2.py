import sys
import numpy as np
import tensorflow as tf
import os
from time import time

# CPU로 강제 실행 (GPU 에러 방지)
os.environ["CUDA_VISIBLE_DEVICES"] = ""

# 명령줄 인자 파싱
if len(sys.argv) != 3:
    print("Usage: python glocalK2.py <training_file> <test_file>")
    sys.exit(1)

training_file = sys.argv[1]  # 예: 'u1.base'
test_file = sys.argv[2]      # 예: 'u1.test'

# 데이터 로드 함수
def load_data(path, train_file, test_file, delimiter='\t'):
    train = np.loadtxt(path + train_file, skiprows=0, delimiter=delimiter).astype('int32')
    test = np.loadtxt(path + test_file, skiprows=0, delimiter=delimiter).astype('int32')
    total = np.concatenate((train, test), axis=0)

    n_u = np.unique(total[:, 0]).size  # 사용자 수
    n_m = np.unique(total[:, 1]).size  # 영화 수
    n_train = train.shape[0]  # 훈련 평점 수
    n_test = test.shape[0]   # 테스트 평점 수

    train_r = np.zeros((n_m, n_u), dtype='float32')
    test_r = np.zeros((n_m, n_u), dtype='float32')

    for i in range(n_train):
        train_r[train[i, 1] - 1, train[i, 0] - 1] = train[i, 2]

    for i in range(n_test):
        test_r[test[i, 1] - 1, test[i, 0] - 1] = test[i, 2]

    train_m = np.greater(train_r, 1e-12).astype('float32')
    test_m = np.greater(test_r, 1e-12).astype('float32')

    print('data matrix loaded')
    print('num of users: {}'.format(n_u))
    print('num of movies: {}'.format(n_m))
    print('num of training ratings: {}'.format(n_train))
    print('num of test ratings: {}'.format(n_test))

    return n_m, n_u, train_r, train_m, test_r, test_m

# 하이퍼파라미터 설정
n_hid = 500
n_dim = 5
n_layers = 2
gk_size = 3
lambda_2 = 20.
lambda_s = 0.006
epoch_p = 30
epoch_f = 60
dot_scale = 1
batch_size = 32

# 데이터 로드
data_path = './'
n_m, n_u, train_r, train_m, test_r, test_m = load_data(data_path, training_file, test_file, delimiter='\t')

# 데이터 제너레이터
def data_generator(ratings, masks, batch_size):
    num_samples = ratings.shape[0]
    indices = np.arange(num_samples)
    while True:
        np.random.shuffle(indices)
        for start_idx in range(0, num_samples, batch_size):
            batch_indices = indices[start_idx:start_idx + batch_size]
            yield ratings[batch_indices], ratings[batch_indices], masks[batch_indices]

train_dataset = tf.data.Dataset.from_generator(
    lambda: data_generator(train_r, train_m, batch_size),
    output_signature=(
        tf.TensorSpec(shape=(None, n_u), dtype=tf.float32),
        tf.TensorSpec(shape=(None, n_u), dtype=tf.float32),
        tf.TensorSpec(shape=(None, n_u), dtype=tf.float32)
    )
).prefetch(tf.data.AUTOTUNE)

# 로컬 커널 레이어
class LocalKernelLayer(tf.keras.layers.Layer):
    def __init__(self, n_hid, n_dim, lambda_s, lambda_2, activation='sigmoid', name=None):
        super(LocalKernelLayer, self).__init__(name=name)
        self.n_hid = n_hid
        self.n_dim = n_dim
        self.lambda_s = lambda_s
        self.lambda_2 = lambda_2
        self.activation = tf.keras.activations.get(activation)

    def build(self, input_shape):
        n_in = input_shape[-1]
        self.W = self.add_weight(name='W', shape=(n_in, self.n_hid),
                                 initializer='glorot_uniform', regularizer=tf.keras.regularizers.l2(self.lambda_2))
        self.u = self.add_weight(name='u', shape=(n_in, 1, self.n_dim),
                                 initializer=tf.keras.initializers.TruncatedNormal(mean=0., stddev=1e-3))
        self.v = self.add_weight(name='v', shape=(1, self.n_hid, self.n_dim),
                                 initializer=tf.keras.initializers.TruncatedNormal(mean=0., stddev=1e-3))
        self.b = self.add_weight(name='b', shape=(self.n_hid,),
                                 initializer='zeros')

    def call(self, inputs):
        dist = tf.norm(self.u - self.v, ord=2, axis=2)
        w_hat = tf.maximum(0., 1. - dist**2)
        self.add_loss(tf.keras.regularizers.l2(self.lambda_s)(w_hat))
        W_eff = self.W * w_hat
        y = tf.matmul(inputs, W_eff) + self.b
        if self.activation:
            y = self.activation(y)
        return y

# 글로벌 커널 레이어
class GlobalKernelLayer(tf.keras.layers.Layer):
    def __init__(self, gk_size, dot_scale, name=None):
        super(GlobalKernelLayer, self).__init__(name=name)
        self.gk_size = gk_size
        self.dot_scale = dot_scale

    def build(self, input_shape):
        n_kernel = input_shape[-1]
        self.conv_kernel = self.add_weight(name='conv_kernel', shape=(n_kernel, self.gk_size**2),
                                          initializer=tf.keras.initializers.TruncatedNormal(stddev=0.1))

    def call(self, inputs):
        avg_pooling = tf.reduce_mean(inputs, axis=0, keepdims=True)
        gk = tf.matmul(avg_pooling, self.conv_kernel) * self.dot_scale
        gk = tf.reshape(gk, [self.gk_size, self.gk_size, 1, 1])
        return gk

# 글로벌 컨볼루션 레이어
class GlobalConvLayer(tf.keras.layers.Layer):
    def __init__(self, name=None):
        super(GlobalConvLayer, self).__init__(name=name)

    def call(self, inputs, kernel):
        x = tf.expand_dims(tf.expand_dims(inputs, 0), -1)
        conv = tf.nn.relu(tf.nn.conv2d(x, kernel, strides=[1,1,1,1], padding='SAME'))
        return tf.squeeze(conv, [0, -1])

# 사전 훈련 모델
def build_pretrain_model(n_m, n_u):
    inputs = tf.keras.Input(shape=(n_u,))
    y = inputs
    for i in range(n_layers):
        y = LocalKernelLayer(n_hid, n_dim, lambda_s, lambda_2, name=f'layer_{i}')(y)
    outputs = LocalKernelLayer(n_u, n_dim, lambda_s, lambda_2, activation=None, name='out')(y)
    model = tf.keras.Model(inputs, outputs)
    return model

# 미세 조정 모델
def build_finetune_model(n_m, n_u, pretrain_model):
    inputs = tf.keras.Input(shape=(n_u,))
    y = inputs
    for i in range(n_layers):
        y = pretrain_model.get_layer(f'layer_{i}')(y)
    y_dash = pretrain_model.get_layer('out')(y)
    gk = GlobalKernelLayer(gk_size, dot_scale)(y_dash)
    y_hat = GlobalConvLayer()(inputs, gk)
    for i in range(n_layers):
        y_hat = LocalKernelLayer(n_hid, n_dim, lambda_s, lambda_2, name=f'ft_layer_{i}')(y_hat)
    outputs = LocalKernelLayer(n_u, n_dim, lambda_s, lambda_2, activation=None, name='ft_out')(y_hat)
    model = tf.keras.Model(inputs, outputs)
    return model

# 손실 함수
def custom_loss(y_true, y_pred, sample_weight=None):
    if sample_weight is None:
        sample_weight = tf.ones_like(y_true)
    diff = sample_weight * (y_true - y_pred)
    loss = tf.reduce_sum(diff ** 2) / tf.reduce_sum(sample_weight)
    return loss

# 평가 함수
def call_rmse_mae(y_hat, y, mask):
    y_hat_clipped = np.clip(y_hat, 1., 5.)
    error = (mask * (y_hat_clipped - y) ** 2).sum() / mask.sum()
    rmse = np.sqrt(error)
    mae = (mask * np.abs(y_hat_clipped - y)).sum() / mask.sum()
    return rmse, mae

# 모델 훈련 및 평가
pretrain_model = build_pretrain_model(n_m, n_u)
pretrain_model.compile(optimizer=tf.keras.optimizers.Adam(learning_rate=0.001), loss=custom_loss)

finetune_model = build_finetune_model(n_m, n_u, pretrain_model)
finetune_model.compile(optimizer=tf.keras.optimizers.Adam(learning_rate=0.001), loss=custom_loss)

best_rmse, best_mae = float("inf"), float("inf")
time_cumulative = 0

# 사전 훈련
for epoch in range(epoch_p):
    tic = time()
    pretrain_model.fit(train_dataset, epochs=1, steps_per_epoch=(n_m // batch_size), verbose=0)
    pre = pretrain_model.predict(train_r, batch_size=batch_size)
    t = time() - tic
    time_cumulative += t

    test_rmse, test_mae = call_rmse_mae(pre, test_r, test_m)
    train_rmse, train_mae = call_rmse_mae(pre, train_r, train_m)

    print('.-^-._' * 12)
    print('PRE-TRAINING')
    print(f'Epoch: {epoch+1}, test rmse: {test_rmse:.4f}, test mae: {test_mae:.4f}')
    print(f'train rmse: {train_rmse:.4f}, train mae: {train_mae:.4f}')
    print(f'Time: {t:.2f} seconds')
    print(f'Time cumulative: {time_cumulative:.2f} seconds')
    print('.-^-._' * 12)

# 미세 조정
for epoch in range(epoch_f):
    tic = time()
    finetune_model.fit(train_dataset, epochs=1, steps_per_epoch=(n_m // batch_size), verbose=0)
    pre = finetune_model.predict(train_r, batch_size=batch_size)
    t = time() - tic
    time_cumulative += t

    test_rmse, test_mae = call_rmse_mae(pre, test_r, test_m)
    train_rmse, train_mae = call_rmse_mae(pre, train_r, train_m)

    if test_rmse < best_rmse:
        best_rmse = test_rmse
    if test_mae < best_mae:
        best_mae = test_mae

    print('.-^-._' * 12)
    print('FINE-TUNING')
    print(f'Epoch: {epoch+1}, test rmse: {test_rmse:.4f}, test mae: {test_mae:.4f}')
    print(f'train rmse: {train_rmse:.4f}, train mae: {train_mae:.4f}')
    print(f'Time: {t:.2f} seconds')
    print(f'Time cumulative: {time_cumulative:.2f} seconds')
    print('.-^-._' * 12)

# 예측 결과 저장
output_file = training_file.replace('.base', '.base_prediction.txt')
with open(output_file, 'w') as f:
    for user in range(n_u):
        for item in range(n_m):
            if test_m[item, user] > 0:
                rating = np.clip(pre[item, user], 1., 5.)
                f.write(f"{user+1}\t{item+1}\t{rating:.2f}\n")

print(f"Predictions saved to {output_file}")
print(f'Best test rmse: {best_rmse:.4f}')
print(f'Best test mae: {best_mae:.4f}')