#include <iostream>
using namespace std;

int main()
{
    int n, k;
    int count = 0;
    cin >> n >> k;
    while (n != 1)
    {
        if (n % k == 0)
        {
            n = n / k;
        }
        else
        {
            n -= 1;
        }
        count++;
    }
    cout << count << endl;
    return 0;
}