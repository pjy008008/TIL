#include <iostream>
#include <string>
#include <cstring>
#include <sstream>
using namespace std;

int main()
{
    int sum = 0;
    string s;
    string word;
    getline(cin, s);
    stringstream sstream(s);
    if (s[0] == ' ')
    {
        getline(sstream, word, ' ');
    }
    while (getline(sstream, word, ' '))
    {
        sum++;
    }
    cout << sum;
    return 0;
}