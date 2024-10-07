#include <stdio.h>

int main()
{
    int minute;
    scanf("%d", &minute);
    if (minute > 120)
    {
        printf("현욱이형 재미없어요..");
    }
    else if (minute > 60)
    {
        printf("들을 만 하네");
    }
    else
    {
        printf("나머지 경우");
    }
    return 0;
}