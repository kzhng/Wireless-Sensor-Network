#include <stddef.h>
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

typedef struct {
    int current_year, current_month, current_day; // date variables
    int current_hour, current_min, current_sec; // time variables
    float latitude, longitude, magnitude, depth; // sensor reading variables
    int my_rank; // rank of process that created record
} Record;

void PrintRecord(Record*);

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

    // create custom MPI datatype for Record
    const int nitems = 11;
    int blocklengths[11] = {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1};
    MPI_Datatype types[11] = {MPI_INT, MPI_INT, MPI_INT, MPI_INT, MPI_INT, MPI_INT, MPI_FLOAT, MPI_FLOAT, MPI_FLOAT, MPI_FLOAT, MPI_INT};
    MPI_Datatype mpi_record_type;
    
    MPI_Aint offsets[11];
    offsets[0] = offsetof(Record, current_year);
    offsets[1] = offsetof(Record, current_month);
    offsets[2] = offsetof(Record, current_day);
    offsets[3] = offsetof(Record, current_hour);
    offsets[4] = offsetof(Record, current_min);
    offsets[5] = offsetof(Record, current_sec);
    offsets[6] = offsetof(Record, latitude);
    offsets[7] = offsetof(Record, longitude);
    offsets[8] = offsetof(Record, magnitude);
    offsets[9] = offsetof(Record, depth);
    offsets[10] = offsetof(Record, my_rank);

    MPI_Type_create_struct(nitems, blocklengths, offsets, types, &mpi_record_type);
    MPI_Type_commit(&mpi_record_type);

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

    // timer to periodically create random records
    clock_t TimeZero = clock();
    double deltaTime = 0;
    double secondsToDelay = 5;
    bool exit = false;

    srand((unsigned int)time(NULL)+my_rank+1);
    
    // TODO: change loop condition
    // TODO: change condition for only slaves to act as nodes and master node to be the base station (for reporting purposes)
    while (!exit) {
        // get delta time in seconds
        deltaTime = (clock() - TimeZero) / CLOCKS_PER_SEC;
        
        // every 5 seconds
        if(deltaTime == secondsToDelay){
            // generate random records
            float base_lat = -15.0;
            float base_long = 167.0;
            float base_mag = 6.0;
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

            // create record 
            Record my_record = {current_year, current_month, current_day,
            current_hour, current_min, current_sec, latitude, longitude, magnitude, depth, my_rank};

            printf("Printing record (rank %d): ", my_record.my_rank);
            PrintRecord(&my_record);
            
            // MPI_Send lat and long to all adjacent processes TODO: check if this is correct
            ierr = MPI_Send(&my_record, 1, mpi_record_type, nbr_i_lo, 0, comm2D);
            ierr = MPI_Send(&my_record, 1, mpi_record_type, nbr_i_hi, 0, comm2D);
            ierr = MPI_Send(&my_record, 1, mpi_record_type, nbr_j_lo, 0, comm2D);
            ierr = MPI_Send(&my_record, 1, mpi_record_type, nbr_j_hi, 0, comm2D);

            // if the generated record magnitude is greater than 3
            if (my_record.magnitude > 3) {
                // send request to adjacent neighbours to acquire their readings and compare
                // TODO: Checking if records are valid. Not eveery node will have a top,bottom etc.
                Record top_record, bottom_record, left_record, right_record;
                ierr = MPI_Recv(&left_record, 1, mpi_record_type, nbr_i_lo, 0, comm2D, &status);
                ierr = MPI_Recv(&right_record, 1, mpi_record_type, nbr_i_hi, 0, comm2D, &status);
                ierr = MPI_Recv(&bottom_record, 1, mpi_record_type, nbr_j_lo, 0, comm2D, &status);
                ierr = MPI_Recv(&top_record, 1, mpi_record_type, nbr_j_hi, 0, comm2D, &status);
                
                // recv from neighbours
                printf("node %d magnitude over 3 (%f). recieved bottom_record from %d: ", my_rank, my_record.magnitude, nbr_j_lo);
                PrintRecord(&bottom_record);
                // compare readings
            }

            //reset the clock timers
            deltaTime = clock();
            TimeZero = clock();
        }
    }


    MPI_Comm_free(&comm2D);
    MPI_Finalize();
    return EXIT_SUCCESS;
}

void set_time_variables() {

}

void PrintRecord(Record *record) {
    printf("%d %d %d %d %d %d %f %f %f %f\n", 
    record->current_year, record->current_month, record->current_day,
    record->current_hour, record->current_min, record->current_sec,
    record->latitude, record->longitude, record->magnitude, record->depth);
}