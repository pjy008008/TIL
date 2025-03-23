// queue_example.cpp
#include <iostream>
#include <queue>

using namespace std;

int main() {
    queue<int> q;
    
    // 요소 추가
    q.push(10);
    q.push(20);
    q.push(30);
    
    cout << "Front element: " << q.front() << endl;
    cout << "Back element: " << q.back() << endl;
    
    // 요소 제거
    q.pop();
    cout << "After pop, front element: " << q.front() << endl;
    
    // 큐가 비었는지 확인
    cout << "Is queue empty? " << (q.empty() ? "Yes" : "No") << endl;
    
    // 큐 크기 확인
    cout << "Queue size: " << q.size() << endl;
    
    return 0;
}