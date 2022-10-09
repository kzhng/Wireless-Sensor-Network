#include <stdio.h>
#include <math.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <mpi.h>

#define SHIFT_ROW 0
#define SHIFT_COL 1
#define DISP 1
#define UPPER 30
#define LOWER 3

void set_time_variables();

int main(int argc, char *argv[]) {
    // mpi variables
    int size, my_rank;
    // cartesian topology variables
    int nrows, ncols; // number rows, cols
    int ndims=2; // number dimensions
    int dims[ndims];
    int coord[ndims];
    int wrap_around[ndims];
    int reorder;
    int ierr;
    int my_cart_rank;
    int nbr_i_lo, nbr_i_hi;
    int nbr_j_lo, nbr_j_hi;
    MPI_Comm comm2D;
    // random generation related variables
    unsigned int seed;

    // setup initial MPI environment
    MPI_Status status;
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    // process command line arguments
    if (argc == 3) {
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

    // 
    // create cartesian topology for processes

    MPI_Dims_create(size, ndims, dims);
    if (my_rank == 0) printf("Root Rank: %d. Comm Size: %d: Grid Dimension = [%d x %d] \n",my_rank,size,dims[0],dims[1]);
    // create cartesian mapping
    wrap_around[0] = wrap_around[1] = 0;
    reorder = 1;
    ierr = 0;
    ierr = MPI_Cart_create(MPI_COMM_WORLD, ndims, dims, wrap_around, reorder, &comm2D);
    // find my coordinates in the cartesian communicator group
    MPI_Cart_coords(comm2D, my_rank, ndims, coord);
    // use my cartesian coordinates to find my rank in cartesian group
    MPI_Cart_rank(comm2D, coord, &my_cart_rank);
    // get my neighbors; axis is coordinate dimension of shift
    /* axis=0 ==> shift along the rows: P[my_row-1]: P[me] :
    P[my_row+1] */
    /* axis=1 ==> shift along the columns P[my_col-1]: P[me] :
    P[my_col+1] */
    MPI_Cart_shift( comm2D, SHIFT_ROW, DISP, &nbr_i_lo, &nbr_i_hi);
    MPI_Cart_shift( comm2D, SHIFT_COL, DISP, &nbr_j_lo, &nbr_j_hi);
    /*printf("Global rank: %d. Cart rank: %d. Coord: (%d, %d). Left: %d. Right: %d. Top: %d. Bottom: %d\n", my_rank,
    my_cart_rank, coord[0], coord[1], nbr_j_lo, nbr_j_hi, nbr_i_lo,
    nbr_i_hi); */
    fflush(stdout);

    // generate random records
    // loop
    srand((unsigned int)time(NULL)+my_rank+1);
    float base_lat = -15.0;
    float base_long = 167.0;
    float base_mag = 4.0;
    float base_depth = 5.0;

    float latitude = ((float)rand()/(float)(RAND_MAX)) * base_lat;
    float longitude = ((float)rand()/(float)(RAND_MAX)) * base_long;
    float magnitude = ((float)rand()/(float)(RAND_MAX)) * base_mag;
    float depth = ((float)rand()/(float)(RAND_MAX)) * base_depth;

    time_t s;
    struct tm* current_time;
    s = time(NULL) + my_rank + 1;
    current_time = localtime(&s);

    int current_year = current_time->tm_year + 1900;
    int current_month = current_time->tm_mon + 1;
    int current_day = current_time->tm_mday;
    int current_hour = current_time->tm_hour;
    int current_min = current_time->tm_min;
    int current_sec = current_time->tm_sec;

    printf("random record for rank(%d): %d %d %d %d %d %d %f %f %f %f\n", my_rank,
    current_year, current_month, current_day, current_hour, current_min, current_sec,
    latitude, longitude, magnitude, depth);

    MPI_Comm_free(&comm2D);
    MPI_Finalize();
    return EXIT_SUCCESS;
}

void set_time_variables() {

}