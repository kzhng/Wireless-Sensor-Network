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
#include <limits.h>
#include "data_structures.h"
#include "balloon.h"
#include "sensor.h"
#include "record.h"
#include "utils.h"

int base_station(MPI_Comm master_comm, MPI_Comm comm, int num_iterations, int nrows, int ncols);
char* getWDay(int wday);
Record findClosestBalloon(Record rep_node, Queue *q);

#endif // BASE_STATION_H