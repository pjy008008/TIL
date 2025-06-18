import pandas as pd
import numpy as np
from sklearn.preprocessing import OneHotEncoder
from sklearn.datasets import dump_svmlight_file
import xlearn as xl
import sys
import os
import tempfile

def load_data(train_file, test_file):
    train_df = pd.read_csv(train_file, sep='\t', names=['user', 'item', 'rating', 'timestamp'])
    test_df = pd.read_csv(test_file, sep='\t', names=['user', 'item', 'rating', 'timestamp'])
    return train_df, test_df

def feature_engineering(train_df, test_df):
    train_df = train_df.drop('timestamp', axis=1)
    test_df = test_df.drop('timestamp', axis=1)

    all_df = pd.concat([train_df, test_df], axis=0)

    encoder = OneHotEncoder(handle_unknown='ignore')
    encoder.fit(all_df[['user', 'item']])

    X_train = encoder.transform(train_df[['user', 'item']])
    X_test = encoder.transform(test_df[['user', 'item']])
    y_train = train_df['rating'].astype(float)
    y_test = test_df['rating'].astype(float)

    return X_train, y_train, X_test, y_test, test_df[['user', 'item']]

def main():
    if len(sys.argv) != 3:
        print("사용법: python fm.py [train_file] [test_file]")
        sys.exit(1)

    train_file = sys.argv[1]
    test_file = sys.argv[2]

    prefix = os.path.splitext(train_file)[0]
    output_filename = f"{prefix}.base_prediction.txt"

    print("1. 데이터 로딩 중...")
    train_df, test_df = load_data(train_file, test_file)
    X_train, y_train, X_test, y_test, user_item_pairs = feature_engineering(train_df, test_df)

    print(f"훈련 데이터 크기: {X_train.shape}")
    print(f"테스트 데이터 크기: {X_test.shape}")

    # 임시 파일 생성
    with tempfile.NamedTemporaryFile(mode='w+', delete=False) as train_file_tmp, \
         tempfile.NamedTemporaryFile(mode='w+', delete=False) as test_file_tmp, \
         tempfile.NamedTemporaryFile(mode='w+', delete=False) as model_file_tmp, \
         tempfile.NamedTemporaryFile(mode='w+', delete=False) as output_file_tmp:

        train_path = train_file_tmp.name
        test_path = test_file_tmp.name
        model_path = model_file_tmp.name
        output_path = output_file_tmp.name

        print("2. libsvm 포맷으로 변환 및 임시 저장...")
        dump_svmlight_file(X_train, y_train, train_path)
        dump_svmlight_file(X_test, y_test, test_path)

        # 하이퍼파라미터 후보군
        learning_rates = [0.01, 0.05, 0.1, 0.2]
        lambdas = [0.0001, 0.001, 0.002, 0.005]
        epochs = [5, 10, 20]

        best_rmse = float('inf')
        best_param = None
        best_preds = None

        print("3. 하이퍼파라미터 튜닝 시작...")
        for lr in learning_rates:
            for lam in lambdas:
                for epoch in epochs:
                    print(f"훈련 중: lr={lr}, lambda={lam}, epoch={epoch}")
                    param = {
                        "task": "reg",
                        "lr": lr,
                        "lambda": lam,
                        "metric": "rmse",
                        "epoch": epoch
                    }

                    fm_model = xl.create_fm()
                    fm_model.setTrain(train_path)
                    fm_model.setValidate(test_path)
                    fm_model.fit(param, model_path)

                    fm_model.setTest(test_path)
                    fm_model.predict(model_path, output_path)

                    preds = np.loadtxt(output_path)
                    rmse = np.sqrt(np.mean((preds - y_test) ** 2))
                    print(f"평가 RMSE: {rmse:.5f}")

                    if rmse < best_rmse:
                        best_rmse = rmse
                        best_param = param.copy()
                        best_preds = preds.copy()

        print(f"최적 하이퍼파라미터: {best_param}, 최저 RMSE: {best_rmse:.5f}")

        user_item_pairs['rating'] = best_preds
        user_item_pairs.to_csv(output_filename, sep='\t', header=False, index=False)
        print(f"최종 결과 저장 완료: {output_filename}")

    # 사용 후 임시 파일 삭제
    for path in [train_path, test_path, model_path, output_path]:
        try:
            os.remove(path)
        except OSError as e:
            print(f"파일 삭제 오류: {path} - {e}")

if __name__ == "__main__":
    main()
