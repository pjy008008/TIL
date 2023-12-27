#include <iostream>
#include <string>
using namespace std;

int main()
{
    int n;
    int sum = 0;
    string s;
    cin >> n >> s;
    for (int i = 0; i < n; i++)
    {
        sum += ((int)s[i] - 48);
    }
    cout << sum;
    return 0;
}