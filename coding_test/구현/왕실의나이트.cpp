#include <iostream>
#include <string>
using namespace std;

int main()
{
    string pos;
    cin >> pos;
    int x, y;
    int nx = 0, ny = 0;
    int count = 0;
    x = pos[0] - 'a' + 1;
    y = pos[1] - '1' + 1;
    int dx[] = {2, 2, -2, -2, 1, -1, 1, -1};
    int dy[] = {1, -1, 1, -1, 2, 2, -2 - 2};
    for (int i = 0; i < 8; i++)
    {
        nx = x + dx[i];
        ny = y + dy[i];
        if (nx <= 0 || nx > 8 || ny <= 0 || ny > 8)
        {
            continue;
        }
        count++;
        nx = 0;
        ny = 0;
    }
    cout << count << endl;
    return 0;
}