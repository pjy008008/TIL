#include <stdio.h>

int main()
{
    char c;
    int a, b;
    printf("두 수 입력: ");
    scanf("%d %d", &a, &b);
    getchar(); // 버퍼에서 개행 문자를 제거

    printf("식 입력 : ");
    scanf(" %c", &c); // 공백 문자를 추가하여 입력 버퍼에서 공백을 건너뛰도록 수정

    switch (c)
    {
    case '+':
        printf("%d", a + b);
        break;
    case '-':
        printf("%d", a - b);
        break; // break 추가
    case '*':
        printf("%d", a * b);
        break; // break 추가
    case '/':
        if (b != 0)
        {
            printf("%d", a / b);
        }
        else
        {
            printf("0으로 나눌 수 없습니다.");
        }
        break; // break 추가
    default:
        printf("올바른 연산자를 입력하세요.");
        break; // break 추가
    }
    return 0;
}
