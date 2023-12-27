#include <iostream>
#include <string>
using namespace std;

int main()
{
    int x;
    int arr[10] = {
        0,
    };
    string s;
    int sum = 1;
    for (int i = 0; i < 3; i++)
    {
        cin >> x;
        sum *= x;
    }
    s = to_string(sum);
    int length = s.length();
    for (int i = 0; i < length; i++)
    {
        arr[s[i]-48]++;
    }
    for (int i = 0; i < 10; i++)
    {
        cout << arr[i] << endl;
    }
    return 0;
}