#include <iostream>
using namespace std;

int main(){
    int x;
    int count=0;
    int arr[45]={0,};
    for(int i=0;i<10;i++){
        cin>>x;
        arr[x%42]++;
    }
    for(int i=0;i<44;i++){
        if(arr[i]!=0){
            count++;
        }
    }
    cout<<count;
    return 0;
}