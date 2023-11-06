#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>

struct Node {
    double data; // 데이터를 double 형식으로 변경
    struct Node* next;
};

// Linked List 관련 함수들의 프로토타입 선언
struct Node* createNode(double data); // createNode 함수도 double 형식을 받도록 변경
void insertNode(struct Node** head, double data); // insertNode 함수도 double 형식을 받도록 변경
bool isPrime(double num);
int countPrimeOperations(struct Node* head);

int main() {
    char str[100];

    FILE* fp = fopen("input.txt", "r");

    if (fp != NULL)
    {
        struct Node* head = NULL;
        double data; // 데이터를 double 형식으로 선언

        while (fgets(str, 100, fp) != NULL)
        {
            data = atof(str); // atof 함수를 사용하여 문자열을 double로 변환
            insertNode(&head, data);
        }
        fclose(fp);

        // 중복되지 않는 데이터의 수를 계산하여 출력
        int totalNodes = 0;
        struct Node* current = head;
        while (current != NULL) {
            totalNodes++;
            current = current->next;
        }
        printf("Only one count : %d\n", totalNodes);

        // 1을 더하거나 빼서 소수가 되는 숫자의 개수를 계산하여 출력
        int primeCount = countPrimeOperations(head);
        printf("Near prime count : %d\n", primeCount);

        // Linked List 메모리 해제
        current = head;
        while (current != NULL) {
            struct Node* temp = current;
            current = current->next;
            free(temp);
        }
    }
    return 0;
}

struct Node* createNode(double data) { // createNode 함수도 double 형식을 받도록 변경
    struct Node* newNode = (struct Node*)malloc(sizeof(struct Node));
    if (newNode == NULL) {
        printf("메모리 할당 오류\n");
        exit(1);
    }
    newNode->data = data;
    newNode->next = NULL;
    return newNode;
}

// Linked List에 노드 삽입 함수 (중복된 데이터는 삽입하지 않음)
void insertNode(struct Node** head, double data) { // insertNode 함수도 double 형식을 받도록 변경
    // Linked List가 비어있는 경우 노드를 추가
    if (*head == NULL) {
        *head = createNode(data);
    }
    else {
        // Linked List가 비어있지 않은 경우 중복된 데이터인지 확인
        struct Node* current = *head;
        while (current != NULL) {
            if (current->data == data) {
                // 중복된 데이터인 경우 노드를 추가하지 않고 반환
                return;
            }
            current = current->next;
        }

        // 중복된 데이터가 아닌 경우 노드를 추가
        struct Node* newNode = createNode(data);
        newNode->next = *head;
        *head = newNode;
    }
}

// 주어진 숫자가 소수인지 확인하는 함수
bool isPrime(double num) { // isPrime 함수도 double 형식을 받도록 변경
    if (num <= 1) {
        return false;
    }
    else if (num!=(int)num) {
        return false;
    }
    for (int i = 2; i <= sqrt(num); i++) {
        if (fmod(num, i) == 0) { // fmod 함수를 사용하여 실수 나머지를 계산
            return false;
        }
    }
    return true;
}

// 1을 더하거나 빼서 소수가 되는 숫자의 개수를 계산하는 함수
int countPrimeOperations(struct Node* head) {
    int primeCount = 0;
    struct Node* current = head;
    while (current != NULL) {
        double num = current->data;
        if (isPrime(num + 1) || isPrime(num - 1)) {
            primeCount++;
        }
        current = current->next;
    }
    return primeCount;
}
