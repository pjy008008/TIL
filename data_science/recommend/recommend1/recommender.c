#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define MAX_USERS 1000
#define MAX_ITEMS 2000
#define K 20  // 잠재 요인 수
#define EPOCHS 30
#define LEARNING_RATE 0.005
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

// 행렬: 사용자 행렬 P, 아이템 행렬 Q
double P[MAX_USERS][K];
double Q[MAX_ITEMS][K];

void init_matrix() {
    for (int i = 0; i < num_users; i++)
        for (int k = 0; k < K; k++)
            P[i][k] = (double)rand() / RAND_MAX;

    for (int i = 0; i < num_items; i++)
        for (int k = 0; k < K; k++)
            Q[i][k] = (double)rand() / RAND_MAX;
}

void load_training_data(const char* filename) {
    FILE* file = fopen(filename, "r");
    if (!file) { perror("Training file error"); exit(1); }

    while (!feof(file)) {
        int user, item;
        double rating;
        if (fscanf(file, "%d\t%d\t%lf\t%*d\n", &user, &item, &rating) == 3) {
            training_data[training_size].user = user - 1;
            training_data[training_size].item = item - 1;
            training_data[training_size].rating = rating;
            training_size++;
        }
    }
    fclose(file);
}

double predict(int user, int item) {
    double dot = 0.0;
    for (int k = 0; k < K; k++)
        dot += P[user][k] * Q[item][k];
    return dot;
}

void train() {
    for (int epoch = 0; epoch < EPOCHS; epoch++) {
        for (int n = 0; n < training_size; n++) {
            int u = training_data[n].user;
            int i = training_data[n].item;
            double r_ui = training_data[n].rating;
            double pred = predict(u, i);
            double e_ui = r_ui - pred;

            // SGD update
            for (int k = 0; k < K; k++) {
                double p_uk = P[u][k];
                double q_ik = Q[i][k];

                P[u][k] += LEARNING_RATE * (e_ui * q_ik - REGULARIZATION * p_uk);
                Q[i][k] += LEARNING_RATE * (e_ui * p_uk - REGULARIZATION * q_ik);
            }
        }
    }
}

void predict_test(const char* test_file, const char* output_file) {
    FILE* fin = fopen(test_file, "r");
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
    const char* test_file = argv[2];

    char output_file[100];
    snprintf(output_file, sizeof(output_file), "%.*s.base_prediction.txt", (int)(strchr(training_file, '.') - training_file), training_file);

    srand(42);
    init_matrix();
    load_training_data(training_file);
    train();
    predict_test(test_file, output_file);

    return 0;
}
