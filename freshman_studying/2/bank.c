#include <stdio.h>

int main()
{
    int money;
    int temp;
    int choice;
    printf("초기 계좌 잔액을 입력하시오 : ");
    scanf("%d", &money);
    while (1)
    {
        printf("1. 잔액 조회\n");
        printf("2. 입금\n");
        printf("3. 출금\n");
        printf("4. 종료\n");
        printf("선택 : ");
        scanf("%d", &choice);
        if (choice == 1)
        {
            printf("현재 잔액은 %d입니다\n", money);
        }
        else if (choice == 2)
        {
            printf("입금할 금액을 입력하세요 : ");
            scanf("%d", &temp);
            money += temp;
            printf("입금이 완료되었습니다. 현재 잔액은 %d원 입니다.\n", money);
        }
        else if (choice == 3)
        {
            printf("출금할 금액을 입력하세요 : ");
            scanf("%d", &temp);
            money -= temp;
            printf("출금이 완료되었습니다. 현재 잔액은 %d원 입니다.\n", money);
        }
        else if (choice == 4)
        {
            printf("프로그램을 종료합니다. 최종 잔액은 %d원 입니다.\n", money);
            break;
        }
        else
        {
            printf("잘못된 입력\n");
            break;
        }
        printf("\n");
    }
}