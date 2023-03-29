#include <iostream>
using namespace std;

int main(){
    int x;
    cin >> x;
    int count =0;
    while (x!=1)
    {
        if(x%3==0){
            x/=3;
            count +=1;
        }
        else if(x%2==0){
            x/=2;
            count +=1;
        }
        else{
            x-=1;
            count +=1;
        }
    }
    cout << count << endl;
    return 0;
}