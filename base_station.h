#ifndef BASE_STATION_H
#define BASE_STATION_H

#define MSG_SEND 1
#define MSG_EXIT 4

#define BALLOON_READINGS_SIZE 10
#define MAX_NBRS 4

#include <mpi.h>
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>
#include "record.h"

int base_station(MPI_Comm master_comm, MPI_Comm comm, int num_iterations, int nrows, int ncols);
char* getWDay(int wday);

typedef struct {
    int capacity;
    int size;
    int front;
    int rear;
    Record *elements;
} Queue;

typedef struct {
    int num_rows;
    int num_cols;
    Queue *q;
} grid_dims;

Queue* createQueue(int maxSize);
void Dequeue(Queue *Q);
Record Front(Queue *Q);
void Enqueue(Queue *Q, Record node);

#endif // BASE_STATION_H