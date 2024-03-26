#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <sys/types.h>

#define BUFFER_SIZE 512

int main()
{
    FILE *fp;
    char buffer[BUFFER_SIZE];
    char last_line[BUFFER_SIZE];
    char *result;
    int special_output_needed = 0;

    // ps -aux 명령 실행
    fp = popen("ps -aux | grep './linuxtown_class1' | grep -v 'grep' | awk '{print $2}'", "r");
    if (fp == NULL)
    {
        perror("popen failed");
        return -1;
    }

    // 명령의 출력에서 PID 읽어오기
    printf("Found PIDs:\n");
    while (fgets(buffer, BUFFER_SIZE, fp) != NULL)
    {
        // PID 출력
        printf("%s", buffer);
        kill(atoi(buffer), SIGUSR1);
    }

    // 파일 포인터 닫기
    pclose(fp);

    // confession.txt 파일 열기
    FILE *confession_file = fopen("../../confession.txt", "r");
    if (confession_file == NULL)
    {
        perror("confession.txt 파일 열기 실패");
        return -1;
    }

    // confession.txt 파일의 마지막 줄 찾기
    while (fgets(buffer, BUFFER_SIZE, confession_file) != NULL)
    {
        strcpy(last_line, buffer); // 마지막 줄 저장
    }

    // 마지막 줄이 "!!!"인지 확인
    result = strstr(last_line, "!!!");
    if (result != NULL)
    {
        // "!!!"를 찾았을 때 특정 문자열 출력
        printf("Special output needed!\n");
        special_output_needed = 1;
    }

    // confession.txt 파일 닫기
    fclose(confession_file);

    return 0;
}
