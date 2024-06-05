#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(int argc, char *argv[])
{
    //N의 값이 없을 경우
    if (argc < 2)
    {
        //N을 입력하세요 출력
        printf("N을 입력하세요.\n");
        //1반환, 종료
        return 1;
    }
    //매개변수로 입력받은 N값 정수로 저장
    int n = atoi(argv[1]);
    //pid변수 선언
    int pid;
    //fork함수 실행
    pid = fork();
    //pid가 0인 경우 자식 프로세스를 의미함
    if (pid == 0)
    {
        //launch child가 N의 값으로 sumfact를 생성할거라고 출력
        printf("launch : start sumfact %d\n", n);
        //execlp함수 실행, 첫 번째 매개변수는 실행하고자 하는 프로그램의 경로
        //두 번째 매개변수부터는 sumfact에 전달할 매개변수들임
        //이때, argv[1]값을 넘겨서 sumfact도 N값을 사용할 수 있게 함.
        execlp("./sumfact", "./sumfact", argv[1], NULL);
    }
    //pid가 0이 아닌 경우 부모 프로세스를 의미함
    else
    {
        //sum변수 선언
        int sum = 0;
        //1부터 n까지 합을 구함
        for (int i = 1; i <= n; i++)
        {
            //합을 구하는 코드
            sum += i;
        }
        //launch parent가 계산한 sum의 값 출력
        printf("launch : sum = %d\n", sum);
        //자식 프로세스의 종료를 기다림
        wait(NULL);
    }
    //종료
    return 0;
}