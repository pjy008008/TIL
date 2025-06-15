import sys
import pandas as pd
from sklearn.metrics.pairwise import cosine_similarity
from sklearn.metrics import mean_squared_error # RMSE 계산을 위한 모듈 임포트
import numpy as np

def predict_rating(user_id, item_id, user_item_matrix, user_similarity_matrix, 
                   global_mean, user_biases, item_biases, k=30):
    """
    사용자-아이템 쌍에 대한 평점을 예측합니다.
    글로벌 평균, 사용자 및 아이템 편향을 고려하여 예측 정확도를 높입니다.

    Args:
        user_id (int): 평점을 예측할 사용자 ID
        item_id (int): 평점을 예측할 아이템 ID
        user_item_matrix (pd.DataFrame): 사용자-아이템 평점 행렬
        user_similarity_matrix (pd.DataFrame): 사용자 간 유사도 행렬
        global_mean (float): 전체 평점의 평균
        user_biases (pd.Series): 각 사용자의 평점 편향 (user_mean - global_mean)
        item_biases (pd.Series): 각 아이템의 평점 편향 (item_mean - global_mean)
        k (int): 예측에 사용할 최대 이웃(유사한 사용자) 수

    Returns:
        float: 예측된 평점
    """
    # 훈련 데이터에 사용자나 아이템이 없는 경우 (Cold Start 문제) 처리
    # 새로운 사용자 또는 아이템의 경우, global_mean으로 예측하고 편향은 0으로 간주합니다.
    # 이 부분은 실제 서비스에서는 더 정교한 콜드 스타트 전략이 필요할 수 있습니다.
    if user_id not in user_item_matrix.index or item_id not in user_item_matrix.columns:
        return global_mean

    # 대상 사용자의 편향 (훈련 데이터에 없으면 0.0)
    bu = user_biases.get(user_id, 0.0)
    # 대상 아이템의 편향 (훈련 데이터에 없으면 0.0)
    bi = item_biases.get(item_id, 0.0)

    # 예측의 기본값 (글로벌 평균 + 사용자 편향 + 아이템 편향)
    base_prediction = global_mean + bu + bi

    # 대상 사용자와 다른 모든 사용자 간의 유사도
    user_similarities = user_similarity_matrix[user_id]

    # 대상 아이템을 평가한 다른 사용자들의 집합
    raters_of_item = user_item_matrix[item_id].dropna().index

    # 대상 아이템을 평가한 사용자 중에서, 대상 사용자와의 유사도만 추출
    # 자기 자신은 유사도 행렬에서 자기 자신과의 유사도가 1이므로 제외하는 것이 합리적입니다.
    sim_of_raters = user_similarities.loc[raters_of_item].drop(user_id, errors='ignore')

    # 유사도가 0보다 큰 상위 k명의 이웃을 선택
    # 유사도와 평점 편차를 계산하기 위해 0보다 큰 유사도만 선택합니다.
    neighbors = sim_of_raters[sim_of_raters > 0].nlargest(k)

    # 예측에 사용할 이웃이 없는 경우, 기본 예측으로 대체
    if neighbors.empty:
        return base_prediction

    # 이웃들이 대상 아이템에 매긴 평점
    neighbor_ratings = user_item_matrix.loc[neighbors.index, item_id]
    
    # 이웃들의 사용자 편향 (fillna(0.0)으로 NaN 값 처리)
    neighbor_user_biases = user_biases.loc[neighbors.index].fillna(0.0)
    
    # 평점 편차 계산: (이웃의 실제 평점) - (글로벌 평균 + 이웃 사용자 편향 + 대상 아이템 편향)
    # 이는 이웃이 해당 아이템에 매긴 평점에서 이웃의 "예상 평점"을 뺀 값입니다.
    deviations = neighbor_ratings - (global_mean + neighbor_user_biases + bi)
    
    # 분자: Σ (유사도 * 평점 편차)
    numerator = np.sum(neighbors * deviations)
    
    # 분모: Σ |유사도|
    denominator = np.sum(np.abs(neighbors))

    # 분모가 0이면 예측 불가, 기본 예측으로 대체
    if denominator == 0:
        return base_prediction

    # 최종 예측 평점 = 기본 예측 + 가중 평균치
    predicted_rating = base_prediction + (numerator / denominator)

    # 평점 범위를 1점에서 5점으로 제한
    predicted_rating = np.clip(predicted_rating, 1, 5)

    return predicted_rating

def main():
    """
    메인 실행 함수
    """
    # 1. 실행 인자 확인
    if len(sys.argv) != 3:
        print("실행 방법: python recommender.py [training_data] [test_data]")
        sys.exit(1)

    training_file = sys.argv[1]
    test_file = sys.argv[2]

    # 2. 데이터 로딩 (timestamp 열은 무시)
    print("1. 데이터 로딩 시작...")
    try:
        train_df = pd.read_csv(training_file, sep='\t', names=['user_id', 'item_id', 'rating', 'timestamp'])
        test_df = pd.read_csv(test_file, sep='\t', names=['user_id', 'item_id', 'rating', 'timestamp'])
    except FileNotFoundError:
        print("오류: 데이터 파일을 찾을 수 없습니다. 파일 경로를 확인하세요.")
        sys.exit(1)

    # 3. 사용자-아이템 평점 행렬 생성
    # 행: user_id, 열: item_id, 값: rating
    print("2. 사용자-아이템 평점 행렬 생성...")
    user_item_matrix = train_df.pivot(index='user_id', columns='item_id', values='rating')

    # 4. 편향(Bias) 계산
    print("3. 글로벌 평균 및 사용자/아이템 편향 계산...")
    global_mean = train_df['rating'].mean()
    user_means = train_df.groupby('user_id')['rating'].mean()
    item_means = train_df.groupby('item_id')['rating'].mean()

    # 편향 = 평균 - 글로벌 평균
    user_biases = user_means - global_mean
    item_biases = item_means - global_mean

    # 5. 사용자 간 유사도 행렬 계산 (피어슨 유사도)
    print("4. 사용자 간 유사도 행렬 계산...")
    # 피어슨 유사도 = 각 사용자의 평점에서 해당 사용자 평균을 뺀 값들 간의 코사인 유사도
    # (1) 각 사용자의 평균 평점으로 평점들을 중앙화 (mean-centering)
    matrix_mean_subtracted = user_item_matrix.subtract(user_item_matrix.mean(axis=1), axis='rows')
    # (2) 비어있는 값(NaN)을 0으로 채움 (유사도 계산을 위해)
    matrix_filled = matrix_mean_subtracted.fillna(0)
    # (3) 코사인 유사도 계산
    user_similarity_matrix = pd.DataFrame(
        cosine_similarity(matrix_filled),
        index=user_item_matrix.index,
        columns=user_item_matrix.index
    )

    # 6. 테스트 데이터에 대한 평점 예측
    print("5. 테스트 데이터에 대한 평점 예측 시작...")
    actual_ratings = []
    predicted_ratings = []
    output_predictions = [] # 파일 저장을 위한 리스트

    for _, row in test_df.iterrows():
        user = row['user_id']
        item = row['item_id']
        actual_rating = row['rating'] # 실제 평점
        
        # 편향 값들을 predict_rating 함수로 전달
        pred = predict_rating(user, item, user_item_matrix, user_similarity_matrix,
                              global_mean, user_biases, item_biases)
        
        actual_ratings.append(actual_rating)
        predicted_ratings.append(pred)
        output_predictions.append((user, item, pred)) # 예측 결과를 파일 저장을 위해 저장

    # 7. RMSE (Root Mean Squared Error) 계산 및 출력
    rmse = np.sqrt(mean_squared_error(actual_ratings, predicted_ratings))
    print(f"\n--- 예측 완료 ---")
    print(f"RMSE (Root Mean Squared Error): {rmse:.4f}")

    # 8. 결과 파일 저장
    output_filename = f"{training_file}_prediction.txt"
    print(f"6. 예측 결과를 '{output_filename}' 파일에 저장 중...")
    with open(output_filename, 'w') as f:
        for p in output_predictions:
            # user_id, item_id, predicted_rating 순으로 탭으로 구분하여 저장
            f.write(f"{p[0]}\t{p[1]}\t{p[2]:.3f}\n")
            
    print(f"성공적으로 예측 완료! 결과가 '{output_filename}' 파일에 저장되었습니다.")


if __name__ == "__main__":
    main()