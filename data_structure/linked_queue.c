#include <stdio.h>
#include <stdlib.h>
#define MAX_QUEUE 10

typedef struct {
	int key;
} element;

typedef struct stack* queuePointer;
typedef struct stack {
	element data;
	queuePointer link;
};
queuePointer front[MAX_QUEUE];
queuePointer rear[MAX_QUEUE];

void addq(int i, element item) {
	queuePointer temp;
	temp = malloc(sizeof(*temp));
	temp->data = item;
	temp->link = NULL;
	if (front[i])
		rear[i]->link = temp;
	else
		front[i] = temp;
	rear[i] = temp;
}

element deleteq(int i) {
	queuePointer temp = front[i];
	element item;
	if (!temp) {
		//queueEmpty
		exit(EXIT_FAILURE);
	}
	item = temp->data;
	front[i] = temp->link;
	free(temp);
	return item;
}

int main() {
	element first = { 1 };
	element second = { 2 };
	element third = { 3 };
	addq(0, first);
	addq(0, second);
	printf("%d\n", front[0]->data); // 1
	printf("%d\n", rear[0]->data); // 2
	deleteq(0);
	printf("%d\n", front[0]->data); // 2
	printf("%d\n", rear[0]->data); // 2
	addq(0, third);
	printf("%d\n", front[0]->data); // 2
	printf("%d\n", rear[0]->data); // 3

	return 0;
}