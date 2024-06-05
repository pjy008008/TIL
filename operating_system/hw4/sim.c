#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <limits.h>
#include <time.h>

// 읽을 수 있는 라인의 수
#define MAX_LINES 100000
// 읽은 수 있는 라인의 길이
#define MAX_LINE_LENGTH 30

// access.list에서 읽은 내용을 저장하기 위한 구조체
typedef struct
{
    // physical page number
    int number;
    // read 혹은 write
    char operation[10];
} Data;

// nru를 수행하기 위한 frame
typedef struct
{
    bool r;       // read bit
    bool m;       // write bit
    int pagenum;  // physical page number가 저장될 변수
    int lru_time; // LRU를 수행하기 위한 시간 저장
} NruData;

int main(int argc, char *argv[])
{
    // 매개변수가 3개가 아닌 경우
    if (argc != 3)
    {
        // 오류메세지 출력 후 종료
        printf("매개변수를 잘못입력하였습니다.\n");
        return 1;
    }

    // 사용자로부터 받는 frame의 개수
    int framenum = atoi(argv[2]);

    // access.list파일 열기
    FILE *file = fopen("access.list", "r");

    // 파일을 열지 못하였을 경우
    if (file == NULL)
    {
        // 오류메세지 반환후 종료
        perror("파일을 열지 못했습니다.\n");
        return 1;
    }

    Data data[MAX_LINES];       // access.list에서 읽은 내용을 저장하기 위한 구조체
    int count = 0;              // 한줄씩 읽을때마다 count기록
    char line[MAX_LINE_LENGTH]; // 한번에 읽을 라인(30개까지 읽을 수 있음)
    int hitcount = 0;           // hit됐을 시 count
    int faultcount = 0;         // fault됐을 시 count
    int readcount = 0;          // read가 읽어난 횟수
    int writecount = 0;         // write가 일어난 횟수

    // 파일 읽기
    while (fgets(line, sizeof(line), file) && count < MAX_LINES)
    {
        // 한줄씩 읽고 data구조체에 저장
        sscanf(line, "%d %s", &data[count].number, data[count].operation);
        // 카운트 값 증가
        count++;
    }
    // 파일 닫기
    fclose(file);

    // 첫번째 매개변수가 fifo인 경우 fifo 수행
    if (strcmp(argv[1], "fifo") == 0)
    {
        int frame[framenum]; // 입력받은 프레임 개수로 배열 생성
        int oldest = 0;      // 가장 오래된 index번호 저장

        // 프레임 초기화 기본 값을 -1로 가짐
        for (int i = 0; i < framenum; i++)
        {
            frame[i] = -1;
        }

        // data에 저장된 값을 읽으며 반복문 수행
        for (int i = 0; i < count; i++)
        {
            // operation이 read일 경우 readcount 증가
            if (strcmp(data[i].operation, "read") == 0)
            {
                readcount++;
            }
            // operation이 write일 경우 writecount 증가
            else if (strcmp(data[i].operation, "write") == 0)
            {
                writecount++;
            }
            // 형식과 다른 문자열이 들어온 경우 스킵
            else
            {
                continue;
            }

            int found = 0; // hit가 됐을 시 표시할 변수

            // 프레임의 개수만큼 반복문 수행
            for (int j = 0; j < framenum; j++)
            {
                // 프레임에 찾고자 하는 physical page number가 있을 경우 hit
                if (frame[j] == data[i].number)
                {
                    // hitcount를 증가시키고 found변수를 1로 변환
                    hitcount++;
                    found = 1;
                    break;
                }
                // 초기화 되어있는 프레임에 physical page number가 올라갈 경우
                if (frame[j] == -1)
                {
                    // faultcount를 증가시키고 found를 1로 변경(아래 내용을 스킵하기 위함)
                    faultcount++;
                    found = 1;
                    frame[j] = data[i].number; // 해당 프레임에 physical page number삽입
                    break;
                }
            }
            // 위의 2경우가 아닌 경우 수행
            if (found == 0)
            {
                faultcount++;                     // page fault카운트 증가
                frame[oldest] = data[i].number;   // 가장오래된 frame에 데이터 저장
                oldest = (oldest + 1) % framenum; // oldest변수 변경 다음 칸으로 넘어감(끝까지 가면 처음으로 돌아감)
            }
        }
    }
    // 첫번째 매개변수가 nru인 경우 nru 수행
    else if (strcmp(argv[1], "nru") == 0)
    {
        NruData frame[framenum]; // 프레임 개수만큼 nrudata구조체 선언

        // nrudata초기화
        for (int i = 0; i < framenum; i++)
        {
            frame[i].pagenum = -1; // pagenum -1로 초기화
            frame[i].r = 0;        // readbit 0으로 초기화
            frame[i].m = 0;        // modifybit 0으로 초기화
            frame[i].lru_time = 0; // LRU를 수행하기 위한 시간 저장 변수 초기화
        }

        // LRU를 수행하기 위한 시간 초기화
        int time = 0;

        // count(data를 읽은 라인)만큼 반복문 수행
        for (int i = 0; i < count; i++)
        {
            time++; // time변수 1씩 증가

            // operation이 read일 경우 readcount증가
            if (strcmp(data[i].operation, "read") == 0)
            {
                readcount++;
            }
            // operation이 write일 경우 writecount 증가
            else if (strcmp(data[i].operation, "write") == 0)
            {
                writecount++;
            }
            // 형식과 다른 문자열이 들어온 경우 스킵
            else
            {
                continue;
            }

            int found = 0; // hit가 났을경우 found값 변경

            // 프레임 개수만큼 반복문 수행
            for (int j = 0; j < framenum; j++)
            {
                // 프레임에 찾고자하는 physical page number가 있는 경우
                if (frame[j].pagenum == data[i].number)
                {
                    hitcount++;               // hit count 증가
                    found = 1;                // found변수 1로 변경
                    frame[j].lru_time = time; // nrudata에 시간 저장

                    // operation이 read일 경우
                    if (strcmp(data[i].operation, "read") == 0)
                    {
                        frame[j].r = 1; // read bit 만 1로 변경
                    }

                    // operation이 write일 경우
                    else if (strcmp(data[i].operation, "write") == 0)
                    {
                        // read bit와 modify bit 모두 1로 변경
                        frame[j].r = 1;
                        frame[j].m = 1;
                    }
                    break;
                }
            }
            // frame에서 physical page number를 찾지 못한 경우
            if (found == 0)
            {
                faultcount++;          // faultcount증가
                int replaceIndex = -1; // replaceIndex -1로 설정 => replaceIndex가 변경되지 않은 경우 확인해야 되기 때문에 -1 로 설정
                int minClass = 5;      // 가장 작은 class가 저장될 변수
                int class = 0; // 현재 프레임의 class가 저장될 변수

                //프레임 개수만큼 반복문 수행
                for (int j = 0; j < framenum; j++)
                {
                    //초기화 한 후 사용되지 않은 경우
                    if (frame[j].pagenum == -1)
                    {
                        //해당 프레임을 replaceIndex로 설정 후 break
                        replaceIndex = j;
                        break;
                    }
                    //클래스 계산 read bit에 2를 곱하고 modify bit를 더함
                    class = 2 * frame[j].r + frame[j].m;

                    //아무 프레임도 선택되지 않았거나, 최소 클래스보다 더 작은 클래스가 있거나
                    //클래스가 minclass와 같으면서 시간이 더 오래되었을 경우(lru_time이 작을수록 오래됨)
                    if (replaceIndex == -1 || class < minClass || (class == minClass && frame[j].lru_time < frame[replaceIndex].lru_time))
                    {
                        //replaceIndex를 변경
                        replaceIndex = j;
                        //최소 class를 현재 class로 설정
                        minClass = class;
                    }
                }

                //바꿀 프레임의 pagenum에 현재 physical page number 삽입
                frame[replaceIndex].pagenum = data[i].number;
                //바꿀 프레임의 lru-time에 현재 시간 삽입
                frame[replaceIndex].lru_time = time;

                //operation이 read인 경우(최초로 프레임에 올라옴)
                if (strcmp(data[i].operation, "read") == 0)
                {
                    //read bit를 0, write bit를 0으로 세팅
                    frame[replaceIndex].r = 0;
                    frame[replaceIndex].m = 0;
                }
                //operation이 write인 경우(최초로 프레임에 올라옴)
                else if (strcmp(data[i].operation, "write") == 0)
                {
                    //read bit를 0, write bit를 1로 세팅
                    frame[replaceIndex].r = 0;
                    frame[replaceIndex].m = 1;
                }
            }
        }
    }
    //fifo또는 nru가 입력되지 않은 경우
    else
    {
        printf("fifo또는 lru를 입력하세요\n");
        return 1;
    }

    //실행 결과 출력
    printf("Total number of access: %d\n", readcount + writecount);
    printf("Total number of read: %d\n", readcount);
    printf("Total number of write: %d\n", writecount);
    printf("Number of page hits: %d\n", hitcount);
    printf("Number of page faults: %d\n", faultcount);
    printf("Page fault rate: %d/%d=%lf%%\n", faultcount, readcount + writecount, ((float)faultcount / (readcount + writecount)) * 100);
    return 0;
}
