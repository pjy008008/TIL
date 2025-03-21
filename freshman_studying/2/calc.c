#include <stdio.h>

int main() {
    char op;  // operator 대신 op로 변경
    double num1, num2, result;

    // 사용자로부터 연산자와 숫자 입력 받기
    printf("Enter an operator (+, -, *, /): ");
    scanf("%c", &op);
    
    printf("Enter two numbers: ");
    scanf("%lf %lf", &num1, &num2);

    // 입력된 연산자에 따라 계산 수행
    switch(op) {  // op로 변경
        case '+':
            result = num1 + num2;
            printf("%.2lf + %.2lf = %.2lf\n", num1, num2, result);
            break;
        case '-':
            result = num1 - num2;
            printf("%.2lf - %.2lf = %.2lf\n", num1, num2, result);
            break;
        case '*':
            result = num1 * num2;
            printf("%.2lf * %.2lf = %.2lf\n", num1, num2, result);
            break;
        case '/':
            if (num2 != 0) {
                result = num1 / num2;
                printf("%.2lf / %.2lf = %.2lf\n", num1, num2, result);
            } else {
                printf("Error! Division by zero.\n");
            }
            break;
        default:
            printf("Invalid operator!\n");
            break;
    }

    return 0;
}
