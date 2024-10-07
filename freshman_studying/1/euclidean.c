#include <stdio.h>

int main()
{
    int a, b, r;
    scanf("%d %d", &a, &b);
    while (b != 0)
    {
        r = a % b;
        a = b;
        b = r;
    }
    if (a == 1)
    {
        printf("두 수는 서로소입니다.");
    }
    else
    {
        printf("최대공약수는 %d입니다.", a);
    }
    return 0;
}