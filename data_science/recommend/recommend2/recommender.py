import sys
import pandas as pd
from sklearn.metrics.pairwise import cosine_similarity
import numpy as np

def predict_rating(user_id, item_id, user_item_matrix, user_similarity_matrix, k=30):
    """
    사용자-아이템 쌍에 대한 평점을 예측합니다.

    Args:
        user_id (int): 평점을 예측할 사용자 ID
        item_id (int): 평점을 예측할 아이템 ID
        user_item_matrix (pd.DataFrame): 사용자-아이템 평점 행렬
        user_similarity_matrix (pd.DataFrame): 사용자 간 유사도 행렬
        k (int): 예측에 사용할 최대 이웃(유사한 사용자) 수

    Returns:
        float: 예측된 평점
    """
    # 훈련 데이터에 사용자나 아이템이 없는 경우 (Cold Start 문제)
    # 전체 평점의 평균값으로 대체
    if user_id not in user_item_matrix.index or item_id not in user_item_matrix.columns:
        return user_item_matrix.stack().mean()

    # 예측 공식에 사용될 대상 사용자의 평균 평점
    user_mean_rating = user_item_matrix.loc[user_id].mean()

    # 대상 사용자와 다른 모든 사용자 간의 유사도
    user_similarities = user_similarity_matrix[user_id]

    # 대상 아이템을 평가한 다른 사용자들의 집합
    raters_of_item = user_item_matrix[item_id].dropna().index

    # 대상 아이템을 평가한 사용자 중에서, 대상 사용자와의 유사도만 추출
    # 자기 자신은 제외
    sim_of_raters = user_similarities.loc[raters_of_item].drop(user_id, errors='ignore')

    # 유사도가 0보다 큰 상위 k명의 이웃을 선택
    neighbors = sim_of_raters[sim_of_raters > 0].nlargest(k)

    # 예측에 사용할 이웃이 없는 경우, 대상 사용자의 평균 평점으로 예측
    if neighbors.empty:
        # 사용자의 평균 평점이 NaN일 경우(평가 기록이 없는 사용자) 전체 평균 사용
        return user_mean_rating if not np.isnan(user_mean_rating) else user_item_matrix.stack().mean()

    # 이웃들이 대상 아이템에 매긴 평점
    neighbor_ratings = user_item_matrix.loc[neighbors.index, item_id]
    
    # 이웃들의 평균 평점
    neighbor_mean_ratings = user_item_matrix.loc[neighbors.index].mean(axis=1)

    # 예측 평점 계산 (가중 평균)
    # 분자: Σ (유사도 * (이웃의 아이템 평점 - 이웃의 평균 평점))
    numerator = np.sum(neighbors * (neighbor_ratings - neighbor_mean_ratings))
    
    # 분모: Σ |유사도|
    denominator = np.sum(np.abs(neighbors))

    # 분모가 0이면 예측 불가, 사용자의 평균 평점으로 대체
    if denominator == 0:
        return user_mean_rating

    # 최종 예측 평점 = 대상 사용자 평균 평점 + 가중 평균치
    predicted_rating = user_mean_rating + (numerator / denominator)

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
    try:
        train_df = pd.read_csv(training_file, sep='\t', names=['user_id', 'item_id', 'rating', 'timestamp'])
        test_df = pd.read_csv(test_file, sep='\t', names=['user_id', 'item_id', 'rating', 'timestamp'])
    except FileNotFoundError:
        print("오류: 데이터 파일을 찾을 수 없습니다. 파일 경로를 확인하세요.")
        sys.exit(1)

    # 3. 사용자-아이템 평점 행렬 생성
    # 행: user_id, 열: item_id, 값: rating
    user_item_matrix = train_df.pivot(index='user_id', columns='item_id', values='rating')

    # 4. 사용자 간 유사도 행렬 계산 (피어슨 유사도)
    # 피어슨 유사도 = 각 사용자의 평점에서 해당 사용자 평균을 뺀 값들 간의 코사인 유사도
    # (1) 각 사용자의 평균 평점으로 평점들을 중앙화 (mean-centering)
    matrix_mean_subtracted = user_item_matrix.subtract(user_item_matrix.mean(axis=1), axis='rows')
    # (2) 비어있는 값(NaN)을 0으로 채움
    matrix_filled = matrix_mean_subtracted.fillna(0)
    # (3) 코사인 유사도 계산
    user_similarity_matrix = pd.DataFrame(
        cosine_similarity(matrix_filled),
        index=user_item_matrix.index,
        columns=user_item_matrix.index
    )

    # 5. 테스트 데이터에 대한 평점 예측
    predictions = []
    for _, row in test_df.iterrows():
        user = row['user_id']
        item = row['item_id']
        pred = predict_rating(user, item, user_item_matrix, user_similarity_matrix)
        predictions.append((user, item, pred))

    # 6. 결과 파일 저장
    output_filename = f"{training_file}_prediction.txt"
    with open(output_filename, 'w') as f:
        for p in predictions:
            # user_id, item_id, predicted_rating 순으로 탭으로 구분하여 저장
            f.write(f"{p[0]}\t{p[1]}\t{p[2]:.3f}\n")
            
    print(f"성공적으로 예측 완료! 결과가 '{output_filename}' 파일에 저장되었습니다.")


if __name__ == "__main__":
    main()