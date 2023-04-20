#include <stdio.h>
#include <stdlib.h>
#define MAX_STACKS 10

typedef struct {
	int key;
} element;

typedef struct stack* stackPointer;
typedef struct stack {
	element data;
	stackPointer link;
};
stackPointer top[MAX_STACKS];

void stackEmpty() {
	printf("stack is empty");
}
void push(int i, element item) {
	stackPointer temp;
	temp = malloc(sizeof(*temp));
	temp->data = item;
	temp->link = top[i];
	top[i] = temp;
}

element pop(int i) {
	stackPointer temp = top[i];
	element item;
	if (!temp) {
		stackEmpty();
		exit(EXIT_FAILURE);
	}
	item = temp->data;
	top[i] = temp->link;
	free(temp);
	return item;
}

int main() {
	element first = { 1 };
	element second = { 2 };
	push(0, first);
	push(0, second);
	printf("%d\n", top[0]->data);  //2
	pop(0);
	printf("%d\n", top[0]->data);  //1

	return 0;
}