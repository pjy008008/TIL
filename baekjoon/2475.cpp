#include <iostream>
using namespace std;

int main()
{
    int x, result = 0;
    for (int i = 0; i < 5; i++)
    {
        cin >> x;
        result += x * x;
    }
    result = result % 10;
    cout <<result;
    return 0;
}