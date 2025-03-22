#include <iostream>
#include <vector>
#include <sstream>
#include <string>
using namespace std;

int main()
{
    int n;
    char s;
    int x = 1, y = 1;
    int count = 0;
    string str;
    vector<char> v;
    cin >> n;
    cin.ignore(); //\n무시
    getline(cin, str);
    istringstream iss(str);
    while (iss >> s)
    {
        count++;
        v.push_back(s);
    }
    for (int i = 0; i < count; i++)
    {
        if (v.at(i) == 'L' && x > 1)
        {
            x--;
        }
        else if (v.at(i) == 'R' && x < n)
        {
            x++;
        }
        else if (v.at(i) == 'U' && y > 1)
        {
            y--;
        }
        else if (v.at(i) == 'D' && y < n)
        {
            y++;
        }
    }
    cout << y << " " << x << endl;
    return 0;
}

// 해설
// #include <bits/stdc++.h>

// using namespace std;

// // N을 입력받기
// int n;
// string plans;
// int x = 1, y = 1;

// // L, R, U, D에 따른 이동 방향
// int dx[4] = {0, 0, -1, 1};
// int dy[4] = {-1, 1, 0, 0};
// char moveTypes[4] = {'L', 'R', 'U', 'D'};

// int main(void) {
//     cin >> n;
//     cin.ignore(); // 버퍼 비우기
//     getline(cin, plans);
//     // 이동 계획을 하나씩 확인
//     for (int i = 0; i < plans.size(); i++) {
//         char plan = plans[i];
//         // 이동 후 좌표 구하기
//         int nx = -1, ny = -1;
//         for (int j = 0; j < 4; j++) {
//             if (plan == moveTypes[j]) {
//                 nx = x + dx[j];
//                 ny = y + dy[j];
//             }
//         }
//         // 공간을 벗어나는 경우 무시
//         if (nx < 1 || ny < 1 || nx > n || ny > n) continue;
//         // 이동 수행
//         x = nx;
//         y = ny;
//     }
//     cout << x << ' ' << y << '\n';
//     return 0;
// }