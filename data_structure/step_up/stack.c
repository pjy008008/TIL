#include <stdio.h>
#define SIZE 100
int stack[SIZE], choice, top;
void push(int x);
int pop();
void display();
int main()
{
    top = -1;
    push(1);

    push(2);

    push(3);

    push(4);

    push(5);

    display();

    pop();

    pop();

    display();

    return 0;
}

void push(int x)
{
    top++;
    stack[top] = x;
}
int pop()
{
    int temp = stack[top];
    top--;
    return temp;
}
void display()
{
    int i = top;
    for (; i >= 0; i--)
    {
        printf("%d\n", stack[i]);
    }
}