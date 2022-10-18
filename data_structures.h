#ifndef DATA_STRUCTURES_H
#define DATA_STRUCTURES_H

#include "balloon.h"
#include "sensor.h"
#include "record.h"
#include "utils.h"

typedef struct {
        time_t alert_time;
        clock_t sending_time;
        int iter_num;
        int nbr_match;
        int nbr_top;
        int nbr_left;
        int nbr_right;
        int nbr_bot;
        Record rep_rec;
        Record top_rec;
        Record left_rec;
        Record right_rec;
        Record bot_rec;
} Report;

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
Record View(Queue *Q, int index);

#endif