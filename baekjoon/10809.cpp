#include <iostream>
#include <string>
using namespace std;

int main()
{
    int arr[26];
    for (int i = 0; i < 26; i++)
    {
        arr[i] = -1;
    }
    string s;
    cin >> s;
    int length = s.length();
    for (int i = 0; i < length; i++)
    {
        if (arr[s[i] - 97] == -1)
        {
            arr[s[i] - 97] = i;
        }
    }
    for (int i = 0; i < 26; i++)
    {
        cout << arr[i] << " ";
    }
    return 0;
}