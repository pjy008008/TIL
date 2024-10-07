#include <stdio.h>

void swap(int *a, int *b)
{
    int tmp;
    tmp = *a;
    *a = *b;
    *b = tmp;
}

int main()
{
    int i, j;
    int min;
    int arr[10] = {10, 30, 90, 20, 100, 70, 60, 80, 50, 40};
    for (i = 0; i < 9; i++)
    {
        min = i;
        for (j = i + 1; j < 10; j++)
        {
            if (arr[j] < arr[min])
            {
                min = j;
            }
        }
        swap(&arr[i], &arr[min]);
    }
    for (i = 0; i < 10; i++)
    {
        printf("%d ", arr[i]);
    }
    return 0;
}