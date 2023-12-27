#include <iostream>
using namespace std;

int main()
{
    int n;
    int x;
    int max, min;
    cin >> n;
    cin >> x;
    max = x;
    min = x;
    for (int i = 1; i < n; i++)
    {
        cin >> x;
        if (x > max)
        {
            max = x;
        }
        else if (x < min)
        {
            min = x;
        }
    }
    cout << min << " " << max;
    return 0;
}