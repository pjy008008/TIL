#include <stdio.h>

int main(void)
{
    int minute;
    printf("수업시간을 입력해 주세요: ");
    scanf("%d", &minute);
    if (minute > 120)
    {
        printf("재미없어요 현욱이형...");
    }
    else
    {
        printf("재미있다.");
    }
    return 0;
}