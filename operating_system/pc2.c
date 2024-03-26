#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#define BUFFER_SIZE 128

int main() {
    pid_t pid;
    FILE *fp;
    char buffer[BUFFER_SIZE];

    // 외부 프로그램 실행하여 출력을 파일에 저장
    system("../../linuxtown_class1 > pid_output.txt");

    // 파일 열기
    fp = fopen("pid_output.txt", "r");
    if (fp == NULL) {
        perror("파일 열기 실패");
        return -1;
    }

    // 파일에서 PID 읽어오기
    while (fgets(buffer, BUFFER_SIZE, fp) != NULL) {
        printf("%s", buffer); // 터미널에 출력
    }

    // 파일 닫기
    fclose(fp);

    // 임시 파일 삭제
    remove("pid_output.txt");

    return 0;
}
