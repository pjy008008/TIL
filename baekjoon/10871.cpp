#include <iostream>
using namespace std;
int main()
{
    int n, x;
    int k;
    cin >> n >> x;
    for (int i = 0; i < n; i++)
    {
        cin >> k;
        if (k < x)
        {
            cout << k << " ";
        }
    }
    return 0;
}