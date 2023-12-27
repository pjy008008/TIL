#include <iostream>
using namespace std;

int main()
{
    int n;
    int x=1;
    int count = 1;
    cin >> n;
    n-=1;
    while (x <= n)
    {
        x += count * 6;
        count++;
    }
    if(n==0){
        cout << 1;
    }
    else{
        cout <<count;
    }
    return 0;
}