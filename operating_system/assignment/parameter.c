#include <stdio.h>

int main(int argc, char *argv[]) {

    if (argc < 2) {
        printf("N을 입력하세요\n");
        return 1;
    }

    printf("전달된 매개변수: %s\n", argv[1]);

    return 0;
}