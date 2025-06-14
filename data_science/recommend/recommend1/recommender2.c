#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define MAX_USERS 1000
#define MAX_ITEMS 2000
#define K 20               // 잠재 요인 수
#define EPOCHS 30          // 학습 epoch 수

// Adam 하이퍼파라미터
#define LEARNING_RATE 0.005
#define BETA1 0.9
#define BETA2 0.999
#define EPSILON 1e-8
#define REGULARIZATION 0.02

int num_users = 943;
int num_items = 1682;

typedef struct {
    int user;
    int item;
    double rating;
} Rating;

Rating training_data[100000];
int training_size = 0;

// 잠재 요인 및 Adam 모멘트
double P[MAX_USERS][K], Q[MAX_ITEMS][K];
double mP[MAX_USERS][K], vP[MAX_USERS][K];
double mQ[MAX_ITEMS][K], vQ[MAX_ITEMS][K];

void init_matrix() {
    // P, Q 난수 초기화 및 m, v 제로 초기화
    for (int i = 0; i < num_users; i++) {
        for (int k = 0; k < K; k++) {
            P[i][k] = ((double)rand() / RAND_MAX - 0.5) * 0.01;
            mP[i][k] = 0.0;
            vP[i][k] = 0.0;
        }
    }
    for (int i = 0; i < num_items; i++) {
        for (int k = 0; k < K; k++) {
            Q[i][k] = ((double)rand() / RAND_MAX - 0.5) * 0.01;
            mQ[i][k] = 0.0;
            vQ[i][k] = 0.0;
        }
    }
}

void load_training_data(const char* filename) {
    FILE* file = fopen(filename, "r");
    if (!file) { perror("Training file error"); exit(1); }
    while (!feof(file)) {
        int user, item;
        double rating;
        if (fscanf(file, "%d\t%d\t%lf\t%*d\n", &user, &item, &rating) == 3) {
            training_data[training_size].user   = user - 1;
            training_data[training_size].item   = item - 1;
            training_data[training_size].rating = rating;
            training_size++;
        }
    }
    fclose(file);
}

double predict(int u, int i) {
    double dot = 0.0;
    for (int k = 0; k < K; k++)
        dot += P[u][k] * Q[i][k];
    return dot;
}

void train() {
    long long t = 0;  // global step
    for (int epoch = 1; epoch <= EPOCHS; epoch++) {
        for (int n = 0; n < training_size; n++) {
            int u = training_data[n].user;
            int i = training_data[n].item;
            double r_ui = training_data[n].rating;
            double pred = predict(u, i);
            double e_ui = r_ui - pred;

            // 각 잠재 요인에 대해 Adam 업데이트
            t++;
            double lr_t = LEARNING_RATE * sqrt(1.0 - pow(BETA2, t)) / (1.0 - pow(BETA1, t));

            for (int k = 0; k < K; k++) {
                // gradient (정규화 항 포함)
                double grad_p = - e_ui * Q[i][k] + REGULARIZATION * P[u][k];
                double grad_q = - e_ui * P[u][k] + REGULARIZATION * Q[i][k];

                // m, v 업데이트 (1차/2차 모멘트)
                mP[u][k] = BETA1 * mP[u][k] + (1.0 - BETA1) * grad_p;
                vP[u][k] = BETA2 * vP[u][k] + (1.0 - BETA2) * grad_p * grad_p;

                mQ[i][k] = BETA1 * mQ[i][k] + (1.0 - BETA1) * grad_q;
                vQ[i][k] = BETA2 * vQ[i][k] + (1.0 - BETA2) * grad_q * grad_q;

                // 파라미터 업데이트
                P[u][k] -= lr_t * (mP[u][k] / (sqrt(vP[u][k]) + EPSILON));
                Q[i][k] -= lr_t * (mQ[i][k] / (sqrt(vQ[i][k]) + EPSILON));
            }
        }
        // (선택) 여기서 검증 RMSE 출력하거나 early stopping 적용 가능
        printf("Epoch %d completed\n", epoch);
    }
}

void predict_test(const char* test_file, const char* output_file) {
    FILE* fin  = fopen(test_file,  "r");
    FILE* fout = fopen(output_file, "w");
    if (!fin || !fout) { perror("Test or output file error"); exit(1); }

    int user, item;
    double rating;
    while (fscanf(fin, "%d\t%d\t%lf\t%*d\n", &user, &item, &rating) == 3) {
        double pred = predict(user - 1, item - 1);
        if (pred < 1.0) pred = 1.0;
        if (pred > 5.0) pred = 5.0;
        fprintf(fout, "%d\t%d\t%.3f\n", user, item, pred);
    }
    fclose(fin);
    fclose(fout);
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        printf("Usage: %s training_file test_file\n", argv[0]);
        return 1;
    }
    const char* training_file = argv[1];
    const char* test_file     = argv[2];

    char output_file[100];
    snprintf(output_file, sizeof(output_file),
             "%.*s.base_prediction.txt",
             (int)(strchr(training_file, '.') - training_file),
             training_file);

    srand(42);
    init_matrix();
    load_training_data(training_file);
    train();
    predict_test(test_file, output_file);
    return 0;
}
