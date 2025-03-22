// #include <iostream>
// using namespace std;
// int main()
// {
//     int n;
//     int sum = 0;
//     cin >> n;
//     if (n >= 23)
//     {
//         sum = 3 * (60 * 60) + (n - 2) * ((15 * 60) + (45 * 15));
//     }
//     else if (n >= 13)
//     {
//         sum = 2 * (60 * 60) + (n - 1) * ((15 * 60) + (45 * 15));
//     }
//     else if (n >= 3)
//     {
//         sum = 60 * 60 + n * ((15 * 60) + (45 * 15));
//     }
//     else if (n == 0)
//     {
//         sum = (15 * 60) + (45 * 15);
//     }
//     else
//     {
//         sum = (n + 1) + ((15 * 60) + (45 * 15));
//     }
//     cout << sum << endl;
//     return 0;
// }

// 두번째 풀이
#include <iostream>
#include <string>
using namespace std;

int main()
{
    int n;
    int count = 0;
    cin >> n;
    string h, m, s;
    string time;
    for (int i = 0; i < n + 1; i++)
    {
        h = to_string(i);
        for (int j = 0; j < 60; j++)
        {
            m = to_string(j);
            for (int k = 0; k < 60; k++)
            {
                s = to_string(k);
                time = h + m + s;
                if (time.find('3') != string::npos)
                {
                    count++;
                }
            }
        }
    }
    cout << count << endl;
}

// 정답
//  #include <bits/stdc++.h>

// using namespace std;

// int h, cnt;

// // 특정한 시각 안에 '3'이 포함되어 있는지의 여부
// bool check(int h, int m, int s) {
//     if (h % 10 == 3 || m / 10 == 3 || m % 10 == 3 || s / 10 == 3 || s % 10 == 3)
//         return true;
//     return false;
// }

// int main(void) {
//     // H를 입력받기
//     cin >> h;
//     for (int i = 0; i <= h; i++) {
//         for (int j = 0; j < 60; j++) {
//             for (int k = 0; k < 60; k++) {
//                 // 매 시각 안에 '3'이 포함되어 있다면 카운트 증가
//                 if (check(i, j, k)) cnt++;
//             }
//         }
//     }
//     cout << cnt << '\n';
//     return 0;
// }