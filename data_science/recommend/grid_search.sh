#!/bin/bash

FOLDS=(u1 u2 u3)
K_VALUES=(5 10 20)
LR_VALUES=(0.01 0.005 0.001)
LAMBDA_VALUES=(0.01 0.02 0.05)

echo "K,LR,LAMBDA,FOLD,RMSE" > result_log.csv

for K in "${K_VALUES[@]}"; do
  for LR in "${LR_VALUES[@]}"; do
    for LAMBDA in "${LAMBDA_VALUES[@]}"; do
      for FOLD in "${FOLDS[@]}"; do

        echo "Running fold=$FOLD with K=$K, LR=$LR, LAMBDA=$LAMBDA"

        # 1. 학습 및 예측 수행
        ./train "$FOLD.base" "$K" "$LR" "$LAMBDA" > /dev/null

        # 2. PA4.exe로 평가 (RMSE 추출)
        OUTPUT=$(./PA4.exe "$FOLD")
        RMSE=$(echo "$OUTPUT" | grep "RMSE:" | awk '{print $2}')

        echo "$K,$LR,$LAMBDA,$FOLD,$RMSE" >> result_log.csv
      done
    done
  done
done
