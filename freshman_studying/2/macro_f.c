#include <stdio.h>
#define SUB(X, Y) X - Y
#define PRT(X) printf("계산 결과는 %d입니다.\n", X)
#define SQR(X) X *X

int main(void)
{
    int result;
    int num_01 = 15, num_02 = 7;
    result = SUB(num_01, num_02);
    PRT(result);
    result = SQR(8);
    PRT(result);
    result = SQR(num_02 + 3);
    PRT(result);

    return 0;
}