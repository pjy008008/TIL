#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <sys/types.h>

#define BUFFER_SIZE 512

int main()
{
    FILE *fp, *fcp;
    char buffer[BUFFER_SIZE];
    int count = 0;
    int line_number = 0;
    int mapia_number = 0;
    int citizen_number = 0;
    int pid[300] = {
        0,
    };
    int mapia[300] = {
        0,
    };

    fp = popen("ps -aux | grep 'linuxtown_class1' | grep -v 'grep' | awk '{print $2}'", "r");
    if (fp == NULL)
    {
        perror("popen failed");
        return -1;
    }

    while (fgets(buffer, BUFFER_SIZE, fp) != NULL)
    {
        pid[count] = atoi(buffer);
        kill(atoi(buffer), SIGUSR1);
        count++;
    }

    sleep(5);
    fcp = fopen("./confession.txt", "r");
    if (fcp == NULL)
    {
        perror("confession file not found");
        return -1;
    }

    while (fgets(buffer, BUFFER_SIZE, fcp) != NULL)
    {

        if (strstr(buffer, "!!!") != NULL)
        {
            mapia[line_number] = 1;
            mapia_number++;
        }
        else
        {
            citizen_number++;
        }
        line_number++;
    }
    printf("mapia = %d\n", mapia_number);
    printf("citizen = %d\n", citizen_number);
    printf("마피아 PID\n");
    for (int i = 0; i < 300; i++)
    {
        if (mapia[i] == 1)
        {
            printf("%d\n", pid[i]);
        }
    }

    pclose(fp);
    fclose(fcp);
    return 0;
}
