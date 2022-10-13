#ifndef BALLOON_H
#define BALLOON_H

#include <pthread.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#define BALLOON_READINGS_SIZE 10

typedef struct {
    int current_year, current_month, current_day; // date variables
    int current_hour, current_min, current_sec; // time variables
    float latitude, longitude, magnitude, depth; // sensor reading variables
    int my_rank; // rank of process that created record
} BalloonRecord;

BalloonRecord balloon_readings[BALLOON_READINGS_SIZE];
int num_readings = 0;
pthread_mutex_t gMutex;

void* balloon(void *pArg);

void PrintBalloonRecord(BalloonRecord *record);
BalloonRecord GenerateBalloonRecord(int sensor_rank);

#endif // BALLOON_H