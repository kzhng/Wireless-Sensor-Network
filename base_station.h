#ifndef BASE_STATION_H
#define BASE_STATION_H

#define MSG_SEND 1
#define MSG_EXIT 2

#include <mpi.h>
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>

int base_station(MPI_Comm master_comm, MPI_Comm comm, int num_iterations);

#endif // BASE_STATION_H