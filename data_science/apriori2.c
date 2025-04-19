#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_TRANSACTIONS 1000
#define MAX_ITEMS_PER_TRANSACTION 100
#define MAX_ITEMS 100
#define MIN_SUPPORT 5

int transactions[MAX_TRANSACTIONS][MAX_ITEMS_PER_TRANSACTION];
int transaction_sizes[MAX_TRANSACTIONS];
int num_transactions = 0;

int all_items[MAX_ITEMS];
int item_count = 0;

void enterData();
void apriori();
void print_itemset(FILE *output, int *items, int size);
int is_subset(int *itemset, int size, int *transaction, int t_size);
int calculate_support(int *itemset, int size);
void generate_subsets(int *items, int n, FILE *output);
int compare(const void *a, const void *b);
int binary_search(int *arr, int size, int target);
void sort_and_unique_items();

int main() {
    enterData();
    sort_and_unique_items();
    apriori();
    return 0;
}

void enterData() {
    FILE *file = fopen("input.txt", "r");
    char line[10000];

    while (fgets(line, sizeof(line), file)) {
        char *token = strtok(line, "\t\n");
        int i = 0;
        while (token != NULL && i < MAX_ITEMS_PER_TRANSACTION) {
            int item = atoi(token);
            transactions[num_transactions][i++] = item;

            int found = 0;
            for (int k = 0; k < item_count; k++) {
                if (all_items[k] == item) {
                    found = 1;
                    break;
                }
            }
            if (!found && item_count < MAX_ITEMS) {
                all_items[item_count++] = item;
            }

            token = strtok(NULL, "\t\n");
        }
        transaction_sizes[num_transactions++] = i;
    }
    fclose(file);
}

int compare(const void *a, const void *b) {
    return (*(int *)a - *(int *)b);
}

void sort_and_unique_items() {
    qsort(all_items, item_count, sizeof(int), compare);
    int unique[MAX_ITEMS];
    int u_count = 0;
    for (int i = 0; i < item_count; i++) {
        if (i == 0 || all_items[i] != all_items[i - 1]) {
            unique[u_count++] = all_items[i];
        }
    }
    memcpy(all_items, unique, u_count * sizeof(int));
    item_count = u_count;
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
    FILE *output = fopen("output2.txt", "w");

    for (int r = 2; r <= item_count; r++) {
        int indices[MAX_ITEMS];
        for (int i = 0; i < r; i++) indices[i] = i;

        while (1) {
            int itemset[MAX_ITEMS];
            for (int i = 0; i < r; i++) itemset[i] = all_items[indices[i]];

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
