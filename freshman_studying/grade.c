#include <stdio.h>

int main()
{
    int grade;
    printf("성적을 입력하세요: ");
    scanf("%d", &grade);
    if (grade == 100)
    {
        printf("만점입니다!");
    }
    else if (grade >= 70)
    {
        printf("A입니다");
    }
    else if (grade >= 30)
    {
        printf("B입니다");
    }
    else
    {
        printf("재수강하세요~");
    }
    return 0;
}