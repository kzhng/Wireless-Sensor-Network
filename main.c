#include <math.h>
#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

#include "sensor.h"
#include "base_station.h"
#include "balloon.h"

int main(int argc, char *argv[]) {
    // mpi variables
    int size, my_rank;
    MPI_Comm sensor_comm;
    // arguments
    int nrows, ncols;
    int dims[NDIMS];
    // number of iterations specified by the user
    int num_iters;
    // initialise MPI Environment
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    // process command line arguments
    if (argc == 4) { // TODO: Take in input for thresholds
        nrows = atoi(argv[1]);
        ncols = atoi(argv[2]);
        num_iters = atoi(argv[3]);
        dims[0] = nrows; // number of rows
        dims[1] = ncols;
        if ((nrows*ncols) != size-1) {
            if (my_rank == 0) printf("Error: nrows (%d)*ncols(%d) = (%d) != size(%d). Adjust nrows*ncols to equal size-1.\n", nrows, ncols, nrows*ncols, size-1);
            MPI_Finalize();
            return EXIT_SUCCESS;
        }
    }
    else {
        nrows=ncols=(int)sqrt(size);
        dims[0]=dims[1]=0;
    }

    int color = my_rank == size - 1; // last process
    MPI_Comm_split(MPI_COMM_WORLD, color, 0, &sensor_comm);

    if (color) {// if last process
        // base station node
        base_station(MPI_COMM_WORLD, sensor_comm, num_iters, nrows, ncols);
    }
    else {
        // sensor node
        sensor_node(MPI_COMM_WORLD, sensor_comm, dims);
    }
        
    MPI_Finalize();
    return EXIT_SUCCESS;
}