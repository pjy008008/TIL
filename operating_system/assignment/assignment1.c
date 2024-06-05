#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/types.h>

int main(int argc, char *argv[])

{
    printf("\n아래는 현재 디렉토리의 파일 리스트입니다.\n\n");
    system("../../linuxtown_class1");
    sleep(5);
    printf("wake up");
    kill(atoi(argv[1]), SIGUSR1);
    system("pkill -SIGUSR1 linuxtown_class1");
    return 0;
}