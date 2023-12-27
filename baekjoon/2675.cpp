#include <iostream>
#include <string>
using namespace std;

int main()
{
    int n;
    int x;
    string s;
    cin >> n;
    for (int i = 0; i < n; i++)
    {
        cin >> x >> s;
        for (int j = 0; j < s.length(); j++)
        {
            for (int k = 0; k < x; k++)
            {
                cout << s[j];
            }
        }
        cout << endl;
    }
    return 0;
}