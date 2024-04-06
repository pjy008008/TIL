#include <stdio.h>

#define SIZE 100

void enqueue(int n);

int dequeue();

void show();

int inp_arr[SIZE];

int Rear = -1;

int Front = -1;

int main()

{

    enqueue(1);

    enqueue(2);

    enqueue(3);

    enqueue(4);

    enqueue(5);

    show();

    dequeue();

    dequeue();

    show();
}

void enqueue(int n)

{

    if (Rear == SIZE - 1)

        printf("Overflow \n");

    else

    {

        if (Front == -1)

            Front = 0;

        Rear = Rear + 1;

        inp_arr[Rear] = n;
    }
}

int dequeue()

{

    if (Front == -1 || Front > Rear)

    {

        printf("Underflow \n");

        return;
    }

    else

    {

        Front = Front + 1;

        return inp_arr[Front - 1];
    }
}

void show()
{

    if (Front == -1)

        printf("Empty Queue \n");

    else
    {

        for (int i = Front; i <= Rear; i++)

            printf("%d \n", inp_arr[i]);

        printf("\n");
    }
}