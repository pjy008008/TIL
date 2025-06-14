import sys
import numpy as np
import pandas as pd
from sklearn.feature_extraction import DictVectorizer
from fastFM import als
from sklearn.metrics import mean_squared_error

def load_data(train_file, test_file):
    # 컬럼명 지정 (tab-separated)
    col_names = ['user', 'item', 'rating', 'timestamp']
    df_train = pd.read_csv(train_file, sep='\t', names=col_names)
    df_test = pd.read_csv(test_file, sep='\t', names=col_names)

    return df_train, df_test

def preprocess(df_train, df_test):
    # FM 모델에 쓸 feature 생성 (user, item 만 사용)
    feature_cols = ['user', 'item']

    dv = DictVectorizer()

    # dict vectorizer에 맞게 변환
    X_train = dv.fit_transform(df_train[feature_cols].to_dict(orient='records'))
    X_test = dv.transform(df_test[feature_cols].to_dict(orient='records'))

    y_train = df_train['rating'].values
    y_test = df_test['rating'].values

    return X_train, y_train, X_test, y_test, dv

def train_and_predict(X_train, y_train, X_test):
    model = als.FMRegression(n_iter=50, rank=10, l2_reg_w=0.1, l2_reg_V=0.1)
    model.fit(X_train, y_train)
    y_pred = model.predict(X_test)
    return y_pred

def save_predictions(df_test, y_pred, test_filename):
    # 테스트 파일명 예: u1.test → u1.base_prediction.txt
    prefix = test_filename.split('.')[0]  # u1
    output_file = f"{prefix}.base_prediction.txt"

    with open(output_file, 'w') as f:
        for user, item, pred_rating in zip(df_test['user'], df_test['item'], y_pred):
            f.write(f"{user}\t{item}\t{pred_rating:.4f}\n")

def main():
    if len(sys.argv) != 3:
        print("Usage: python recommender.py [training_data] [test_data]")
        sys.exit(1)

    train_file = sys.argv[1]
    test_file = sys.argv[2]

    # 데이터 불러오기
    df_train, df_test = load_data(train_file, test_file)

    # 전처리
    X_train, y_train, X_test, y_test, dv = preprocess(df_train, df_test)

    # 학습 및 예측
    y_pred = train_and_predict(X_train, y_train, X_test)

    # 결과 저장
    save_predictions(df_test, y_pred, test_file)

    # Optional: RMSE 출력
    rmse = np.sqrt(mean_squared_error(y_test, y_pred))
    print(f"[RMSE] {rmse:.4f}")

if __name__ == "__main__":
    main()
