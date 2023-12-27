#include <iostream>
using namespace std;

int main()
{
    int x, y;
    while (1)
    {
        cin >> x >> y;
        if (cin.eof())
        {
            break;
        }
        else
            cout << x + y << endl;
    }
    return 0;
}