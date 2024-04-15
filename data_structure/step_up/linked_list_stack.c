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

    push(3);

    push(4);

    print_s();
}

void push(int data)
{

    stack *new_node = (stack *)malloc(sizeof(stack));

    if (new_node == NULL)
    {

        printf("stack is full!");

        exit(1);
    }

    new_node->data = data;

    new_node->next = top;

    top = new_node;
}

int pop()
{

    stack *del;

    int value;

    if (top == NULL)
    {

        printf("stack is empty!");

        exit(1);
    }

    value = top->data;

    del = top;

    top = top->next;

    free(del);

    return value;
}

void print_s()
{

    stack *i = top;

    // stack *count = top;

    // i = count;

    while (i != NULL)
    {

        printf("%d \n", i->data);

        i = i->next;

        // count = count->next;
    }

    printf("\n");
}