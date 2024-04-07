#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX 100

// 파싱 테이블
int action_tbl[12][6] = {
    {5, 0, 0, 4, 0, 0},
    {0, 6, 0, 0, 0, 999},
    {0, -2, 7, 0, -2, -2},
    {0, -4, -4, 0, -4, -4},
    {5, 0, 0, 4, 0, 0},
    {0, -6, -6, 0, -6, -6},
    {5, 0, 0, 4, 0, 0},
    {5, 0, 0, 4, 0, 0},
    {0, 6, 0, 0, 11, 0},
    {0, -1, 7, 0, -1, -1},
    {0, -3, -3, 0, -3, -3},
    {0, -5, -5, 0, -5, -5}
};

int goto_tbl[12][4] = {
    {0, 1, 2, 3},
    {0, 0, 0, 0},
    {0, 0, 0, 0},
    {0, 0, 0, 0},
    {0, 8, 2, 3},
    {0, 0, 0, 0},
    {0, 0, 9, 3},
    {0, 0, 0, 10},
    {0, 0, 0, 0},
    {0, 0, 0, 0},
    {0, 0, 0, 0},
    {0, 0, 0, 0}
};

char lhs[] = {' ', 'E', 'E', 'T', 'T', 'F', 'F'}; // 더미 인덱스 0
int rhs_len[] = {0, 3, 1, 3, 1, 3, 1};           // 더미 규칙에 대한 길이
char token[] = {'d', '+', '*', '(', ')', '$'};
char NT[] = {' ', 'E', 'T', 'F'}; // 더미 인덱스 0

int stack[MAX], sp;

void shift(int state) {
    stack[++sp] = state;
}

void reduce(int rule) {
    int len = rhs_len[rule];
    sp -= len;
    int non_terminal = lhs[rule];
    int goto_state = goto_tbl[stack[sp]][non_terminal - 'E']; // E는 1부터 시작하므로 인덱스 조정
    stack[++sp] = goto_state;
}

void LR_Parser(char input[]) {
    sp = 0;
    stack[sp] = 0; // 초기 상태는 스택에 0을 넣음
    int ip = 0;    // input 포인터

    printf("(파싱 단계) (파서 동작) (스택 내용)\n");

    while (1) {
        int state = stack[sp];
        char lookahead = input[ip];

        // 파싱 단계 출력
        printf("(%d) ", state);

        int token_index = -1;
        for (int i = 0; i < strlen(token); i++) {
            if (lookahead == token[i]) {
                token_index = i;
                break;
            }
        }

        if (token_index == -1) { // 입력된 문자가 토큰에 없는 경우
            printf("Error ");
            break;
        }

        if (action_tbl[state][token_index] > 0) { // shift 동작
            shift(action_tbl[state][token_index]);
            printf("Shift ");
            ip++;
        } else if (action_tbl[state][token_index] < 0) { // reduce 동작
            int rule = -action_tbl[state][token_index];
            reduce(rule);
            printf("Reduce by %c -> ", lhs[rule]);
            for (int i = 0; i < rhs_len[rule]; i++) {
                printf("%c", NT[stack[sp + i]]);
            }
            printf(" ");
        } else if (action_tbl[state][token_index] == 0) { // 에러
            printf("Error ");
            break;
        }

        // 스택 내용 출력
        for (int i = 0; i <= sp; i++) {
            printf("%d ", stack[i]);
        }
        printf("\n");

        if (action_tbl[state][token_index] == 999) { // accept 동작
            printf("Accepted\n");
            break;
        }
    }
}

int main() {
    char input[MAX+1]; // 입력 배열 크기를 1 더 늘림
    while (1) {
        printf("\nInput: ");
        scanf("%s", input);
        strcat(input, "$"); // 입력 끝에 '$' 추가
        if (input[0] == '$')
            break;
        LR_Parser(input);
    }
    return 0;
}
