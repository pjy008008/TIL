#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>

int main(int argc, char *argv[])
{

    if (argc < 2)
    {
        printf("N을 입력하세요.\n");
        return 1;
    }
    int n;
    n = atoi(argv[1]);
    int pid;
    pid = fork();
    if (pid == 0)
    {
        // child
        printf("launch : start sumfact %d\n", n);
    }
    else
    {
        // parent
        int sum = 0;
        for (int i = 1; i <= n; i++)
        {
            sum += i;
        }
        printf("launch : sum = %d\n", sum);
    }
    return 0;
}