#include <iostream>
#include <string>
#include <algorithm>
#include <math.h>
using namespace std;

int main()
{
    int t;
    int n, h, w;
    int hh;
    double ww;
    cin >> t;
    for (int i = 0; i < t; i++)
    {
        cin >> h >> w >> n;
        hh = n % h;
        if (hh == 0)
        {
            hh = h;
        }
        int count = 0;
        while (n > 0)
        {
            n -= h;
            count++;
        }
        ww = count;
        // ww = n / h;
        // ww = floor(ww) + 1;
        // if (ww > w)
        // {
        //     ww = w;
        // }
        if (ww < 10)
        {
            cout << hh << 0 << ww << endl;
        }
        else
        {
            cout << hh << ww << endl;
        }
    }

    return 0;
}