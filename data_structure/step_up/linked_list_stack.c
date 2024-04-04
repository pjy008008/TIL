#include <stdio.h>
#include <stdlib.h>

void push(int);
int pop();
void print_s();

typedef struct node
{
    int data;
    struct node *next;
} stack;

stack *top = NULL;

int main()
{
    push(1);
    push(2);
    push(3);
    push(4);
    push(5);
    print_s();

    pop();
    pop();
    print_s();

    return 0;
}

void push(int data)
{
    stack *temp = (stack*)malloc(sizeof(stack));
    if (temp == NULL) {
        printf("Memory allocation failed.\n");
        return;
    }
    temp->data = data;
    temp->next = top;
    top = temp;
}

int pop()
{
    if (top == NULL) {
        printf("Stack is empty.\n");
        return -1;
    }
    int data = top->data;
    stack *temp = top;
    top = top->next;
    free(temp);
    return data;
}

void print_s()
{
    if (top == NULL) {
        printf("Stack is empty.\n");
        return;
    }
    stack *current = top;
    while (current != NULL) {
        printf("%d\n", current->data);
        current = current->next;
    }
}
