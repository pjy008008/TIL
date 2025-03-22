#include <iostream>
using namespace std;

int main()
{
    int n, m;
    int a, b, dir;
    int count = 1;
    int movecount = 0;
    cin >> n >> m;
    cin >> a >> b >> dir;
    int x;
    int map[n][m];
    int dx[] = {0, -1, 0, 1};
    int dy[] = {-1, 0, 1, 0};
    int nx = 0, ny = 0;
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < m; j++)
        {
            cin >> x;
            map[i][j] = x;
        }
    }
    while (1)
    {
        if (movecount == 4)
        {
            a = a - dx[dir];
            b = b - dy[dir];
            if (map[b][a] == 1)
            {
                break;
            }
        }
        dir = dir - 1;
        if (dir == -1)
        {
            dir = 3;
        }
        nx = a + dx[dir];
        ny = b + dy[dir];
        if (map[ny][nx] == 1)
        {
            movecount++;
            nx = 0;
            ny = 0;
            continue;
        }
        movecount = 0;
        map[b][a] = 1;
        a = nx;
        b = ny;
        nx = 0;
        ny = 0;

        count++;
    }
    cout << count << endl;
    return 0;
}
