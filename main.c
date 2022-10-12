#include <math.h>
#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

#include "sensor.h"
#include "base_station.h"

int main(int argc, char *argv[]) {
    // mpi variables
    int size, my_rank;
    MPI_Comm new_comm;
    // cartesian topology variables
    int nrows, ncols;
    int ndims=2;
    int dims[ndims];
    // initialise MPI Environment
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    // process command line arguments
    if (argc == 3) { // TODO: Take in input for thresholds
        nrows = atoi(argv[1]);
        ncols = atoi(argv[2]);
        dims[0] = nrows; // number of rows
        dims[1] = ncols;
        if ((nrows*ncols) != size) {
            if (my_rank == 0) printf("Error: nrows (%d)*ncols(%d) = (%d) != size(%d). Adjust nrows*ncols to equal size.\n", nrows, ncols, nrows*ncols, size);
            MPI_Finalize();
            return EXIT_SUCCESS;
        }
    }
    else {
        nrows=ncols=(int)sqrt(size);
        dims[0]=dims[1]=0;
    }
    int color = my_rank == size - 1; // last process
    MPI_Comm_split(MPI_COMM_WORLD, color, 0, &new_comm);

    if (color) {// if last process
        // base station node
        base_station(MPI_COMM_WORLD, new_comm);
    }
    else {
        // sensor node
        sensor_node(MPI_COMM_WORLD, new_comm);
    }
        
    MPI_Finalize();
    return EXIT_SUCCESS;
}