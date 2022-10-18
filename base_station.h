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
#include "data_structures.h"

int base_station(MPI_Comm master_comm, MPI_Comm comm, int num_iterations, int nrows, int ncols);
char* getWDay(int wday);

#endif // BASE_STATION_H