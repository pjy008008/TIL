#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define MAX_LINE_LENGTH 10000
#define MAX_ITEMS 10000
#define MIN_SUPPORT 4

int **transactions = NULL;
int *transaction_sizes = NULL;
int num_transactions = 0;

void enterData();
void freeData();
void apriori();
void print_itemset(FILE *output, int *items, int size);
int is_subset(int *itemset, int size, int *transaction, int t_size);
int calculate_support(int *itemset, int size);
void generate_subsets(int *items, int n, FILE *output);

int main() {
    enterData();
    apriori();
    freeData();
    return 0;
}

void enterData() {
    FILE *file = fopen("input.txt", "r");

    char line[MAX_LINE_LENGTH];
    while (fgets(line, sizeof(line), file)) {
        line[strcspn(line, "\n")] = 0;

        int num_items = 0;
        char *p = line;
        while (*p) {
            if (*p == '\t') num_items++;
            p++;
        }
        if (p > line) num_items++;

        int *items = (int *)malloc(num_items * sizeof(int));
        char *token = strtok(line, "\t");
        int i = 0;
        while (token != NULL) {
            items[i++] = atoi(token);
            token = strtok(NULL, "\t");
        }

        transactions = (int **)realloc(transactions, (num_transactions + 1) * sizeof(int *));
        transaction_sizes = (int *)realloc(transaction_sizes, (num_transactions + 1) * sizeof(int));
        transactions[num_transactions] = items;
        transaction_sizes[num_transactions] = num_items;
        num_transactions++;
    }

    fclose(file);
}

void freeData() {
    for (int i = 0; i < num_transactions; i++) {
        free(transactions[i]);
    }
    free(transactions);
    free(transaction_sizes);
}

int is_subset(int *itemset, int size, int *transaction, int t_size) {
    for (int i = 0; i < size; i++) {
        int found = 0;
        for (int j = 0; j < t_size; j++) {
            if (itemset[i] == transaction[j]) {
                found = 1;
                break;
            }
        }
        if (!found) return 0;
    }
    return 1;
}

int calculate_support(int *itemset, int size) {
    int count = 0;
    for (int i = 0; i < num_transactions; i++) {
        if (is_subset(itemset, size, transactions[i], transaction_sizes[i])) {
            count++;
        }
    }
    return count;
}

void print_itemset(FILE *output, int *items, int size) {
    fprintf(output, "{");
    for (int i = 0; i < size; i++) {
        if (i > 0) fprintf(output, ",");
        fprintf(output, "%d", items[i]);
    }
    fprintf(output, "}");
}

int compare(const void *a, const void *b) {
    return (*(int *)a - *(int *)b);
}

void generate_subsets(int *items, int n, FILE *output) {
    int total = 1 << n;
    for (int mask = 1; mask < total - 1; mask++) {
        int lhs[MAX_ITEMS], rhs[MAX_ITEMS];
        int lhs_size = 0, rhs_size = 0;

        for (int i = 0; i < n; i++) {
            if (mask & (1 << i)) lhs[lhs_size++] = items[i];
            else rhs[rhs_size++] = items[i];
        }

        int support_count = calculate_support(items, n);
        double support = 100.0 * support_count / num_transactions;

        int lhs_count = calculate_support(lhs, lhs_size);
        if (lhs_count == 0) continue;
        double confidence = 100.0 * support_count / lhs_count;

        if (support >= MIN_SUPPORT) {
            print_itemset(output, lhs, lhs_size);
            fprintf(output, "\t");
            print_itemset(output, rhs, rhs_size);
            fprintf(output, "\t%.2f\t%.2f\n", support, confidence);
        }
    }
}

void apriori() {
    FILE *output = fopen("output.txt", "w");
    int all_items[MAX_ITEMS], item_count = 0;
    for (int i = 0; i < num_transactions; i++) {
        for (int j = 0; j < transaction_sizes[i]; j++) {
            int item = transactions[i][j];
            int found = 0;
            for (int k = 0; k < item_count; k++) {
                if (all_items[k] == item) {
                    found = 1;
                    break;
                }
            }
            if (!found) all_items[item_count++] = item;
        }
    }

    for (int r = 2; r <= item_count; r++) {
        int indices[r];
        for (int i = 0; i < r; i++) indices[i] = i;

        while (1) {
            int itemset[r];
            for (int i = 0; i < r; i++) itemset[i] = all_items[indices[i]];
            qsort(itemset, r, sizeof(int), compare);

            int support_count = calculate_support(itemset, r);
            double support = 100.0 * support_count / num_transactions;

            if (support >= MIN_SUPPORT) {
                generate_subsets(itemset, r, output);
            }

            int i;
            for (i = r - 1; i >= 0; i--) {
                if (indices[i] != i + item_count - r) {
                    indices[i]++;
                    for (int j = i + 1; j < r; j++) {
                        indices[j] = indices[j - 1] + 1;
                    }
                    break;
                }
            }
            if (i < 0) break;
        }
    }
    fclose(output);
}
