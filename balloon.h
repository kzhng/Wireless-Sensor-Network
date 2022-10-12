#ifndef BALLOON_H
#define BALLOON_H

#include <pthread.h>

int balloon(MPI_Comm master_comm, MPI_Comm comm);

#endif // BALLOON_H