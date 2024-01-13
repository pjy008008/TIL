#include <iostream>
#include <string>
#include <algorithm>
#include <vector>
using namespace std;
int main()
{
    int x;
    int length;
    bool isPalin = true;
    string s;
    while (1)
    {
        cin >> x;
        if (x == 0)
        {
            break;
        }
        s = to_string(x);
        length = s.length();
        for (int i = 0; i < length / 2; i++)
        {
            if (s[i] != s[length - i - 1])
            {
                isPalin = false;
            }
        }
        if (isPalin == true)
        {
            cout << "yes" << endl;
        }
        else
        {
            cout << "no" << endl;
        }
        isPalin = true;
    }

    return 0;
}