#include <stdio.h>

int add(int a, int b)
{
    return a + b;
}
int sub(int a, int b)
{
    return a - b;
}
int mul(int a, int b)
{
    return a * b;
}
int div(int a, int b)
{
    if (b == 0)
    {
        printf("계산 불가\n");
        return 0;
    }
    return a / b;
}
void calculator()
{

    while (1)
    {
        char c;
        int a, b;
        int result;
        printf("연산자를 입력해주세요(q는 탈출):");
        c = getchar();
        getchar();
        if (c == 'q')
        {
            printf("종료\n");
            break;
        }
        printf("두 수를 입력해주세요:");
        scanf("%d %d", &a, &b);
        getchar();

        switch (c)
        {
        case '+':
            result = add(a, b);
            break;
        case '-':
            result = sub(a, b);
            break;
        case '*':
            result = mul(a, b);
            break;
        case '/':
            result = div(a, b);
            break;
        }
        printf("결과값은 %d입니다.\n", result);
    }
}

int main()
{
    calculator();
    return 0;
}