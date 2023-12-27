#include <iostream>
#include <vector>
using namespace std;
int main()
{
    int x;
    vector<int> v;
    int inc = 0, dec = 0;
    for (int i = 0; i < 8; i++)
    {
        cin >> x;
        v.push_back(x);
    }
    for (int i = 1; i < 8; i++)
    {
        if (v[i] > v[i - 1])
        {
            inc++;
        }
        else if (v[i] < v[i - 1])
        {
            dec++;
        }
    }
    if (inc == 7)
    {
        cout << "ascending";
    }
    else if (dec == 7)
    {
        cout << "descending";
    }
    else
    {
        cout << "mixed";
    }
}