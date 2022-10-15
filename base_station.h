#ifndef BASE_STATION_H
#define BASE_STATION_H

#include <mpi.h>
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>

int base_station(MPI_Comm master_comm, MPI_Comm comm, int num_iterations);

#endif // BASE_STATION_H