// stack_example.cpp
#include <iostream>
#include <stack>

using namespace std;

int main() {
    stack<int> s;
    
    // 요소 추가
    s.push(100);
    s.push(200);
    s.push(300);
    
    cout << "Top element: " << s.top() << endl;
    
    // 요소 제거
    s.pop();
    cout << "After pop, top element: " << s.top() << endl;
    
    // 스택이 비었는지 확인
    cout << "Is stack empty? " << (s.empty() ? "Yes" : "No") << endl;
    
    // 스택 크기 확인
    cout << "Stack size: " << s.size() << endl;
    
    return 0;
}
