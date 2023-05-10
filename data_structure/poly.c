#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h> // delete

int main()
{
    char str1[200000]; //200000
    char str2[200000]; //200000
    int sum[10001]; //10001
    clock_t start = clock(); //delete
    FILE *pFile = fopen("final4.txt", "r");
    fgets(str1, 200000, pFile);
    fgets(str2, 200000, pFile);
    fclose(pFile);
    char *ptr1 = strtok(str1, "(),");
    while (ptr1 != NULL)
    {
        int coef = atoi(ptr1);
        ptr1 = strtok(NULL, "(),");
        if (ptr1 == NULL)
        {
            break;
        }
        int exp = atoi(ptr1);
        sum[exp] = coef;
        ptr1 = strtok(NULL, "(),");
    }
    char *ptr2 = strtok(str2, "(),");
    while (ptr2 != NULL)
    {
        int coef = atoi(ptr2);
        ptr2 = strtok(NULL, "(),");
        if (ptr2 == NULL)
        {
            break;
        }
        int exp = atoi(ptr2);
        sum[exp] += coef;
        ptr2 = strtok(NULL, "(),");
    }
    int isFirst = 1;
    for (int i = 10000; i >= 0; i--)
    {
        if (sum[i] != 0)
        {
            if (!isFirst)
            {
                printf(" + ");
            }
            printf("%dx^%d", sum[i], i);
            isFirst = 0;
            // if (!isFirst) strcat(output, " + ");
            // char temp[64];
            // sprintf(temp, "%dx^%d", sum[i], i);
            // strcat(output, temp);
            // isFirst = 0;
        }
    }
    clock_t end = clock();                                    // delete
    printf("\n%lf", (double)(end - start) / ((clock_t)1000)); // delete
    return 0;
}