#include <stdio.h>

int main()
{
    int grade;
    grade = 20;
    do
    {
        scanf("%d", &grade);
        printf("%d\n", grade);
    } while (grade < 10);

    return 0;
}