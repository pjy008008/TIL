import sys
import pandas as pd
import numpy as np
import tensorflow as tf
from tensorflow.keras.models import Model
from tensorflow.keras.layers import Input, Embedding, Flatten, Dense, Concatenate, Dropout
from sklearn.preprocessing import StandardScaler, LabelEncoder

def load_and_preprocess_data(training_file, test_file):
    """
    u.data, u.user, u.item 파일을 로드하고 모델 학습에 맞게 전처리합니다.
    """
    # 평점 데이터 로드
    train_df = pd.read_csv(training_file, sep='\t', names=['user_id', 'item_id', 'rating', 'timestamp'])
    test_df = pd.read_csv(test_file, sep='\t', names=['user_id', 'item_id', 'rating', 'timestamp'])

    # 사용자 정보 로드 및 전처리
    u_user = pd.read_csv('u.user', sep='|', names=['user_id', 'age', 'gender', 'occupation', 'zip_code'])
    u_user['gender'] = LabelEncoder().fit_transform(u_user['gender'])
    u_user['occupation'] = LabelEncoder().fit_transform(u_user['occupation'])
    u_user['age'] = StandardScaler().fit_transform(u_user[['age']])
    user_features = u_user.set_index('user_id')[['age', 'gender', 'occupation']]

    # 아이템(영화) 정보 로드 및 전처리
    # --- THIS IS THE CORRECTED PART ---
    u_item = (pd.read_csv('u.item', sep='|', encoding='latin-1',
                          names=['item_id', 'title'] + [str(i) for i in range(22)])
              .iloc[:, [0] + list(range(-19, 0))])
    # ------------------------------------
    u_item.columns = ['item_id', 'unknown', 'Action', 'Adventure', 'Animation', "Children's", 'Comedy',
                      'Crime', 'Documentary', 'Drama', 'Fantasy', 'Film-Noir', 'Horror', 'Musical',
                      'Mystery', 'Romance', 'Sci-Fi', 'Thriller', 'War', 'Western']
    item_features = u_item.set_index('item_id')

    # 평점 데이터와 사용자/아이템 정보 병합
    train_df = train_df.join(user_features, on='user_id').join(item_features, on='item_id')
    test_df = test_df.join(user_features, on='user_id').join(item_features, on='item_id')

    return train_df, test_df, user_features, item_features

def build_hybrid_model(num_users, num_items, user_feature_shape, item_feature_shape, embedding_dim=64):
    """
    사용자/아이템의 고유 ID와 부가 정보를 모두 사용하는 딥러닝 모델을 구축합니다.
    """
    # 1. 사용자 모델 부분 (User Tower)
    user_id_input = Input(shape=(1,), name='user_id_input')
    user_embedding = Embedding(input_dim=num_users + 1, output_dim=embedding_dim, name='user_embedding')(user_id_input)
    user_vec = Flatten(name='flatten_user_embedding')(user_embedding)

    user_features_input = Input(shape=(user_feature_shape,), name='user_features_input')
    
    user_representation = Concatenate()([user_vec, user_features_input])
    user_representation = Dense(128, activation='relu')(user_representation)
    user_representation = Dropout(0.3)(user_representation)

    # 2. 아이템 모델 부분 (Item Tower)
    item_id_input = Input(shape=(1,), name='item_id_input')
    item_embedding = Embedding(input_dim=num_items + 1, output_dim=embedding_dim, name='item_embedding')(item_id_input)
    item_vec = Flatten(name='flatten_item_embedding')(item_embedding)
    
    item_features_input = Input(shape=(item_feature_shape,), name='item_features_input')

    item_representation = Concatenate()([item_vec, item_features_input])
    item_representation = Dense(128, activation='relu')(item_representation)
    item_representation = Dropout(0.3)(item_representation)

    # 3. 최종 평점 예측
    concat = Concatenate()([user_representation, item_representation])
    dense = Dense(128, activation='relu')(concat)
    dropout = Dropout(0.3)(dense)
    output = Dense(1, name='output')(dropout)

    model = Model(inputs=[user_id_input, user_features_input, item_id_input, item_features_input], outputs=output)
    model.compile(optimizer='adam', loss='mean_squared_error')
    
    return model

def main():
    if len(sys.argv) != 3:
        print("실행 방법: python ghrs.py [training_data] [test_data]")
        sys.exit(1)

    training_file = sys.argv[1]
    test_file = sys.argv[2]
    
    print("1. 데이터 로딩 및 전처리 시작...")
    try:
        train_df, test_df, user_features, item_features = load_and_preprocess_data(training_file, test_file)
    except FileNotFoundError as e:
        print(f"오류: {e.filename} 파일을 찾을 수 없습니다. u.user, u.item 파일이 있는지 확인하세요.")
        sys.exit(1)

    num_users = max(train_df['user_id'].max(), test_df['user_id'].max())
    num_items = max(train_df['item_id'].max(), test_df['item_id'].max())

    # 모델 학습을 위한 입력 데이터 준비
    X_train = {
        'user_id_input': train_df['user_id'].values,
        'user_features_input': train_df[user_features.columns].values,
        'item_id_input': train_df['item_id'].values,
        'item_features_input': train_df[item_features.columns].values
    }
    y_train = train_df['rating'].values

    print("2. 딥러닝 모델 구축 및 학습 시작...")
    model = build_hybrid_model(
        num_users,
        num_items,
        user_feature_shape=len(user_features.columns),
        item_feature_shape=len(item_features.columns)
    )
    
    model.fit(X_train, y_train, epochs=10, batch_size=256, verbose=1)

    print("3. 테스트 데이터에 대한 평점 예측 시작...")
    X_test = {
        'user_id_input': test_df['user_id'].values,
        'user_features_input': test_df[user_features.columns].values,
        'item_id_input': test_df['item_id'].values,
        'item_features_input': test_df[item_features.columns].values
    }
    predictions = model.predict(X_test).flatten()

    output_filename = f"{training_file}_prediction.txt"
    result_df = pd.DataFrame({
        'user_id': test_df['user_id'],
        'item_id': test_df['item_id'],
        'rating': predictions
    })
    
    result_df['rating'] = result_df['rating'].clip(1, 5)
    result_df.to_csv(output_filename, sep='\t', header=False, index=False, float_format='%.3f')

    print(f"성공적으로 예측 완료! 결과가 '{output_filename}' 파일에 저장되었습니다.")

if __name__ == "__main__":
    main()