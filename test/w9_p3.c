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

bool check_prime(int i);

int main(int argc, char *argv[]) {
    int ndims=2, size, my_rank, reorder, my_cart_rank, ierr;
    int nrows, ncols;
    int nbr_i_lo, nbr_i_hi;
    int nbr_j_lo, nbr_j_hi;
    MPI_Comm comm2D;
    int dims[ndims],coord[ndims];
    int wrap_around[ndims];
    unsigned int seed;
    int my_prime;

    int errclass, resultlen;
    char err_buffer[MPI_MAX_ERROR_STRING];

    /* start up initial MPI environment */
    MPI_Status status;
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

    /* process command line arguments*/
    if (argc == 3) {
        nrows = atoi (argv[1]);
        ncols = atoi (argv[2]);
        dims[0] = nrows; /* number of rows */
        dims[1] = ncols; /* number of columns */
        if( (nrows*ncols) != size) {
            if( my_rank ==0) printf("ERROR: nrows*ncols)=%d * %d = %d != %d\n", nrows, ncols, nrows*ncols,size);
            MPI_Finalize();
            return 0;
    }
    } else {
        nrows=ncols=(int)sqrt(size);
        dims[0]=dims[1]=0;
    }
    /************************************************************
    */
    /* create cartesian topology for processes */
    /************************************************************
    */
    MPI_Dims_create(size, ndims, dims);
    if(my_rank==0)
        printf("Root Rank: %d. Comm Size: %d: Grid Dimension = [%d x %d] \n",my_rank,size,dims[0],dims[1]);
    /* create cartesian mapping */
    wrap_around[0] = wrap_around[1] = 0; /* periodic shift is
    .false. */
    reorder = 1;
    ierr =0;
    ierr = MPI_Cart_create(MPI_COMM_WORLD, ndims, dims,
    wrap_around, reorder, &comm2D);
    if(ierr != 0) printf("ERROR[%d] creating CART\n",ierr);
    /* find my coordinates in the cartesian communicator group */
    MPI_Cart_coords(comm2D, my_rank, ndims, coord);
    /* use my cartesian coordinates to find my rank in cartesian
    group*/
    MPI_Cart_rank(comm2D, coord, &my_cart_rank);
    /* get my neighbors; axis is coordinate dimension of shift */
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
    
    FILE *fp;
    char output_str[50];
    sprintf(output_str,"process_%d.txt", my_rank);
    
    fp = fopen(output_str, "w");

    if (fp == NULL){
        printf("file can't be opened\n");
        MPI_Finalize();
        return EXIT_FAILURE;
    }

    int left_num = 0;
    int right_num = 0;
    int top_num = 0;
    int bot_num = 0;
    seed = time(NULL) + my_rank + 1;
    
    for (int i=0; i < 10; i++) { // fix number 10
        bool is_prime = false;
        while (!is_prime) {
            my_prime = (rand_r(&seed) % (UPPER - LOWER + 1)) + LOWER;
            if (check_prime(my_prime))
                is_prime = true;
        }
        //printf("my prime is %d at process %d\n", my_prime, my_rank);
        
        /* MPI_Send self prime number to all adjacent processes */
        ierr = MPI_Send(&my_prime, 1, MPI_INT, nbr_i_lo, 0, comm2D);
        ierr = MPI_Send(&my_prime, 1, MPI_INT, nbr_i_hi, 0, comm2D);
        ierr = MPI_Send(&my_prime, 1, MPI_INT, nbr_j_lo, 0, comm2D);
        ierr = MPI_Send(&my_prime, 1, MPI_INT, nbr_j_hi, 0, comm2D);
        /* MPI_Recv all the prime numbers of the adjacent processes */
        ierr = MPI_Recv(&left_num, 1, MPI_INT, nbr_i_lo, 0, comm2D, &status);
        ierr = MPI_Recv(&right_num, 1, MPI_INT, nbr_i_hi, 0, comm2D, &status);
        ierr = MPI_Recv(&bot_num, 1, MPI_INT, nbr_j_lo, 0, comm2D, &status);
        ierr = MPI_Recv(&top_num, 1, MPI_INT, nbr_j_hi, 0, comm2D, &status);
        

        /* if conditions to check whether adjacent processes have the same prime number */
        if (my_prime == left_num) 
            // printf("matched primes with %d and %d at %d and %d\n", my_prime, left_num, my_rank, nbr_i_lo);
            fprintf(fp, "At iteration %d, process %d matched prime number %d with process %d\n", i, my_rank, my_prime, nbr_i_lo);
            // file write stuff
        if (my_prime == right_num)
            // printf("matched primes with %d and %d at %d and %d\n", my_prime, right_num, my_rank, nbr_i_hi);
            fprintf(fp, "At iteration %d, process %d matched prime number %d with process %d\n", i, my_rank, my_prime, nbr_i_hi);
        if (my_prime == top_num)
            // printf("matched primes with %d and %d at %d and %d\n", my_prime, top_num, my_rank, nbr_j_hi);
            fprintf(fp, "At iteration %d, process %d matched prime number %d with process %d\n", i, my_rank, my_prime, nbr_j_hi);
        if (my_prime == bot_num)
            fprintf(fp, "At iteration %d, process %d matched prime number %d with process %d\n", i, my_rank, my_prime, nbr_j_lo);
            // printf("matched primes with %d and %d at %d and %d\n", my_prime, bot_num, my_rank, nbr_j_lo);
    }

    fclose(fp);

    MPI_Comm_free( &comm2D );
    MPI_Finalize();
    return EXIT_SUCCESS;
}

bool check_prime(int i) {
    int k;
    bool is_prime = true;
    for (k=2; k<= sqrt(i);k++) {
        if (i % k == 0) {
            is_prime = false;
            return is_prime;
        }
    }
    return is_prime;
}
