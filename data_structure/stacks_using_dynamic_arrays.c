#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
typedef struct {
	int key;
}element;
element* stack;
int capacity = 1;
int top = -1;
bool isEmpty() {
	if (top < 0)
		return true;
	else
		return false;
}
void stackEmpty() {
	printf("stack is empty");
}
void stackFull() {
	stack = realloc(stack, 2 * capacity * sizeof(*stack));
	capacity *= 2;
}
void push(int item) {
	if (top >= capacity) {
		stackFull();
	}
	(stack+(++top))->key = item;
}
int pop() {
	if (top == -1) {
		stackEmpty();
		exit(EXIT_FAILURE);
	}
	return (stack + (top--))->key;
}
int main() {
	stack = malloc(sizeof(*stack));
	push(1);
	push(2);
	printf("%d\n", pop()); // 2
	printf("%d\n", pop()); // 1
	printf("%d\n", pop()); // stack is empty
	return 0;
}