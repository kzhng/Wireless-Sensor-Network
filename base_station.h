#ifndef BASE_STATION_H
#define BASE_STATION_H

#include <mpi.h>
#include <pthread.h>

int base_station(MPI_Comm master_comm, MPI_Comm comm);

#endif // BASE_STATION_H