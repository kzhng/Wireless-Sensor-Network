#include "data_structures.h"

Queue* createQueue(int maxSize)
{
    /*Creation of a queue*/
    Queue *Q;
    Q = (Queue *)malloc(sizeof(Queue));

    /*Properties of the queue*/
    
    Q->elements = malloc(sizeof(Record)*maxSize);
    Q->capacity = maxSize;
    Q->size = 0;
    Q->front = 0;
    Q->rear = -1;
    
    return Q;
}

void Dequeue(Queue *Q) {
    if (Q->size == 0) {
        printf("Hey! The queue seems to be empty!\n");
        return;
    } else {
        Q->size--;
        Q->front++;
    }
    /*Circular structure of the array-based Queue*/
    if (Q->front == Q->capacity) {
        Q->front = 0;
    }
    return;
}

Record Front(Queue *Q) {
    if (Q->size == 0) {
        printf("Hey! The queue seems to be empty!\n");
        exit(0);
    }
    return Q->elements[Q->front];
}

void Enqueue(Queue *Q, Record node) {
    if (Q->size == Q->capacity) {
        printf("Hey! The queue is too full!\n");

    } else {
        Q->size++;
        Q->rear++;
        /*Circular structure of the array based queue*/
        if(Q->rear == Q->capacity) {
            Q->rear = 0;
        }
        Q->elements[Q->rear] = node;

    }
    return;
}

Record View(Queue *Q, int index) {
    if (Q->size == 0) {
        printf("Hey! The queue seems to be empty!\n");
        exit(0);
    }
    if (index > Q->size - 1) {
        printf("invalid index\n");
    }

    return Q->elements[index];
}