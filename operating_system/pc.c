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

    return 0;
}
