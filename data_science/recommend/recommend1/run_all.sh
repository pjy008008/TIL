#!/bin/bash

total_rmse=0.0

for i in {1..3}
do
    train="u${i}.base"
    test="u${i}.test"
    output="u${i}.base_prediction.txt"

    echo "Running Fold $i..."
    ./recommender.exe $train $test

    # RMSE 계산
    rmse=$(paste $test $output | awk '
    {
        real = $3
        pred = $7
        sum += (real - pred) * (real - pred)
        count++
    }
    END {
        printf "%.4f\n", sqrt(sum / count)
    }')

    echo "Fold $i RMSE: $rmse"
    total_rmse=$(echo "$total_rmse + $rmse" | bc)
done

avg_rmse=$(echo "$total_rmse / 3" | bc -l)
echo "Average RMSE over 3 folds: $(printf "%.4f\n" $avg_rmse)"
