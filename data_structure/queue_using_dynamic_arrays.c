#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#define MAX_QUEUE_SIZE 10
typedef struct {
	int key;
}element;
element* queue;
int rear = -1;
int front = -1;
int capacity = 1;
void queueEmpty() {
	printf("queue is empty");
}
void queueFull() {
	printf("full");
	element* newQueue;
	newQueue = malloc(2 * capacity * sizeof(*queue));
	int start = (front + 1) % capacity;
	if (start < 2)
		memcpy(queue + start, queue + start + capacity - 1, newQueue);
	else {
		memcpy(queue + start, queue + capacity, newQueue);
		memcpy(queue, queue + rear + 1, newQueue + capacity - start);
	}
	front = 2 * capacity - 1;
	rear = capacity - 2;
	capacity *= 2;
	free(queue);
	queue = newQueue;
}

void addq(int item) {
	rear = (rear + 1) % MAX_QUEUE_SIZE;
	if(front==rear)
		queueFull();
	// queue[rear] = item;
	(queue + rear)->key = item;
}
int deleteq() {
	if (front == rear) {
		queueEmpty();
		exit(EXIT_FAILURE);
	}
	front++;
	return (queue + front)->key;
}
int main() {
	queue = malloc(sizeof(*queue)*MAX_QUEUE_SIZE);
	addq(3);
	addq(5);
	printf("%d", deleteq()); // 3
	printf("%d", deleteq()); // 5
	printf("%d", deleteq()); // queue is empty
	free(queue);
	return 0;
}