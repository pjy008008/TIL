#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <errno.h>

//N의 값에 thread들도 접근할 수 있게 전역변수로 선언
int n;

void *threadsumfact(void *data)
{
    //매개 변수로 받은 data는 첫 번째로 생성된 thread의 id임
    int firstThread = *((int *)data);
    //생성되고 있는 thread의 id확인
    pthread_t self_id = pthread_self();
    //만약 현재 thread가 첫 번째로 생성된 thread라면 팩토리얼 구하는 코드 실행
    if (pthread_equal(self_id, *((pthread_t *)data)))
    {
        //최대한 큰 수의 factorial을 구할 수 있게 longlongint로 선언함, 하지만 한계가 있음
        long long int factorialSum = 1;
        //iterative하게 factorial을 구함
        for (int i = 1; i <= n; i++)
        {
            //factorial값을 구하는 코드
            factorialSum *= i;
        }
        //구한 factorial의 값을 출력함
        printf("sumfact : factorial = %lld\n", factorialSum);
    }
    //만약 현재 thread가 첫 번째로 생성된 thread가 아니라면 sum을 구하는 코드 실행
    else
    {
        //sum을 저장할 변수 선언
        int sum = 0;
        //sum을 구하는 코드
        for (int i = 1; i <= n; i++)
        {
            //sum을 구하는 코드
            sum += i;
        }
        //구한 sum의 값을 출력함
        printf("sumfact : sum = %d\n", sum);
    }
    pthread_exit(NULL);
}

int main(int argc, char *argv[])
{
    //N이 입력되지 않았을 경우 종료
    if (argc < 2)
    {
        //종료
        return 1;
    }
    //launch로 부터 받은 N값을 정수로 저장
    n = atoi(argv[1]);

    //N개의 thread선언
    pthread_t thread_id[n];
    //N개의 thread를 생성하는 코드
    for (int i = 0; i < n; i++)
    {
        //thread생성, 각 thread의 id가 저장되고, thread의 특성은 기본값
        //threadsumfact라는 함수를 실행, 첫번째로 생성된 thread의 id를 매개 변수로 전달
        pthread_create(&thread_id[i], NULL, threadsumfact, (void *)&thread_id[0]);
    }
    //N개의 thread가 종료될 때까지 대기하는 코드
    for (int i = 0; i < n; i++)
    {
        //for문을 돌면서 모든 thread가 종료될 때까지 대기
        pthread_join(thread_id[i], NULL);
    }
    //종료
    return 0;
}