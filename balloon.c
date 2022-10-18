#include "balloon.h"
#include "data_structures.h"
#include "base_station.h"

extern pthread_mutex_t gMutex;

void* balloon(void *input) {
    clock_t TimeZero = clock();
    double deltaTime = 0;
    double secondsToDelay = 2;
    bool exit = false;
    int index = 0;
    int i;
    int nrows=0, ncols=0;
    int queue_size=0;
    Queue *balloonQueue;

    float min_lat=0, max_lat=0, min_long=0, max_long=0;

    nrows = ((grid_dims*)input)->num_rows;
    ncols = ((grid_dims*)input)->num_cols;
    balloonQueue = ((grid_dims*)input)->q;

    // determines the bounds for coordinates in location in which the balloon readings can be generated at
    min_lat = find_min_coord(ORIGIN_LATITUDE);
    max_lat = find_max_coord(ORIGIN_LATITUDE, ncols);
    min_long = find_min_coord(ORIGIN_LONGITUDE);
    max_long = find_max_coord(ORIGIN_LONGITUDE, nrows);

    // printf("hello world %d %d %f %f %f %f\n", nrows, ncols, min_lat, max_lat, min_long, max_long);
    while (!exit) {
        deltaTime = (clock() - TimeZero) / CLOCKS_PER_SEC;
        
        // every 5 seconds
        if(deltaTime == secondsToDelay){
            // generate random records
            Record my_record = GenerateBalloonRecord(min_lat, max_lat, min_long, max_long);
            // printf("BALLOON PRINTING RECORD\n");
            // PrintRecord(&my_record);
            pthread_mutex_lock(&gMutex);
            queue_size = balloonQueue->size;
            if (queue_size >= BALLOON_READINGS_SIZE) {
                Dequeue(balloonQueue);
            }
            // adds balloon reading to queue
            Enqueue(balloonQueue, my_record);
            pthread_mutex_unlock(&gMutex);

            //reset the clock timers
            deltaTime = clock();
            TimeZero = clock();
        }

    }
    // printf("hello jupiter\n");
    return NULL;
}

float find_min_coord(float base_coord) {
    return (float)(base_coord - SENSOR_BLOCK_SIZE);
}

float find_max_coord(float base_coord, float component) {
    return (float)(base_coord + SENSOR_BLOCK_SIZE * (component + 1));
}