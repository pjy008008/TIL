#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
using namespace std;

int main()
{
    int n, length;
    int sum = 0;
    int arr[81] = {
        0,
    };
    string s;
    cin >> n;
    for (int i = 0; i < n; i++)
    {
        cin >> s;
        length = s.length();
        for (int j = 0; j < length; j++)
        {
            if (s[j] == 'O')
            {
                if (j == 0)
                {
                    arr[j] = 1;
                }
                else
                {
                    arr[j] = arr[j - 1] + 1;
                }
            }
            else
            {
                arr[j] = 0;
            }
        }
        for (int j = 0; j < 81; j++)
        {
            sum += arr[j];
            arr[j] = 0;
        }
        cout << sum << endl;
        sum = 0;
    }
    return 0;
}