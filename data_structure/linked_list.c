#include <stdio.h>
#include <stdlib.h>
#define IS_EMPTY(first)(!(first))

typedef struct listNode* listPointer;
typedef struct listNode {
	int data;
	listPointer link;
};
//x 는 temp가 삽입될 위치를 나타낸다.
void insert(listPointer* first, listPointer x) {
	listPointer temp;
	temp = malloc(sizeof(*temp));
	temp->data = 50;
	if (*first) {
		temp->link = x->link;
		x->link = temp;
	}
	else {
		temp->link = NULL;
		*first = temp;
	}
}
//trail은 삭제될 x의 선행 노드이다
void delete(listPointer* first, listPointer trail, listPointer x) {
	if (trail) {
		trail->link = x->link;
	}
	else {
		*first = (*first)->link;
	}
	free(x);
}

void printList(listPointer first) {
	printf("This list contains: ");
	for (; first; first = first->link) {
		printf("%4d", first->data);
	}
	printf("\n");
}

int main() {
	listPointer first;
	
	return 0;
}