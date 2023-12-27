
#include <iostream>
using namespace std;

int main()
{
    int x, max = 0;
    int maxnum;
    for (int i = 1; i <= 9; i++)
    {
        cin >> x;
        if (x > max)
        {
            max = x;
            maxnum = i;
        }
    }
    cout << max << endl
         << maxnum;
    return 0;
}