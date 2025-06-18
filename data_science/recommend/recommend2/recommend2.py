import pandas as pd
import numpy as np
from sklearn.preprocessing import OneHotEncoder
from sklearn.metrics import mean_squared_error
from lightfm import LightFM
from lightfm.data import Dataset
import sys
import os

def load_data(train_file, test_file):
    train_df = pd.read_csv(train_file, sep='\t', names=['user', 'item', 'rating', 'timestamp'])
    test_df = pd.read_csv(test_file, sep='\t', names=['user', 'item', 'rating', 'timestamp'])
    return train_df.drop(columns='timestamp'), test_df.drop(columns='timestamp')

def prepare_dataset(train_df, test_df):
    all_users = pd.concat([train_df['user'], test_df['user']])
    all_items = pd.concat([train_df['item'], test_df['item']])
    
    dataset = Dataset()
    dataset.fit(all_users, all_items)  # 모든 user/item 등록

    (interactions, _) = dataset.build_interactions(list(zip(train_df['user'], train_df['item'], train_df['rating'])))
    (test_interactions, _) = dataset.build_interactions(list(zip(test_df['user'], test_df['item'], test_df['rating'])))

    return dataset, interactions, test_interactions

def evaluate_model(model, dataset, test_df):
    test_preds = []
    true_ratings = []

    user_map, _, item_map, _ = dataset.mapping()

    for _, row in test_df.iterrows():
        uid = user_map.get(row['user'])
        iid = item_map.get(row['item'])
        if uid is not None and iid is not None:
            pred = model.predict(uid, iid)
            test_preds.append(pred[0])
            true_ratings.append(row['rating'])

    rmse = np.sqrt(mean_squared_error(true_ratings, test_preds))
    return rmse, test_preds

def main():
    if len(sys.argv) != 3:
        print("사용법: python recommend_lightfm.py [train_file] [test_file]")
        sys.exit(1)

    train_file = sys.argv[1]
    test_file = sys.argv[2]

    output_filename = os.path.splitext(train_file)[0] + ".base_prediction_lightfm.txt"

    print("1. 데이터 로딩 중...")
    train_df, test_df = load_data(train_file, test_file)
    dataset, interactions, _ = prepare_dataset(train_df, test_df)

    print("2. 하이퍼파라미터 튜닝 중...")
    learning_rates = [0.01, 0.05, 0.1, 0.2]
    epochs = [5, 10, 20]
    best_rmse = float('inf')
    best_model = None
    best_preds = None
    best_params = None

    for lr in learning_rates:
        for epoch in epochs:
            print(f"훈련 중: learning_rate={lr}, epoch={epoch}")
            model = LightFM(loss='mse', learning_rate=lr, no_components=20)
            model.fit(interactions, epochs=epoch, num_threads=4)

            rmse, preds = evaluate_model(model, dataset, test_df)
            print(f"평가 RMSE: {rmse:.5f}")

            if rmse < best_rmse:
                best_rmse = rmse
                best_model = model
                best_preds = preds
                best_params = {'lr': lr, 'epoch': epoch}

    print(f"최적 파라미터: {best_params}, 최저 RMSE: {best_rmse:.5f}")

    test_df['predicted_rating'] = best_preds
    test_df[['user', 'item', 'predicted_rating']].to_csv(output_filename, sep='\t', header=False, index=False)
    print(f"최종 결과 저장 완료: {output_filename}")

if __name__ == "__main__":
    main()
