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
double deg2rad(double);
double distance(double lat1, double lon1, double lat2, double lon2);

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
    // printf("Global rank: %d. Cart rank: %d. Coord: (%d, %d). Left: %d. Right: %d. Top: %d. Bottom: %d\n", my_rank,
    // my_cart_rank, coord[0], coord[1], nbr_j_lo, nbr_j_hi, nbr_i_lo,
    // nbr_i_hi); 
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

            printf("rank (%d)(1) Printing record: ", my_record.my_rank);
            PrintRecord(&my_record);
            
            // Only send record to valid adjacent neighbours
            if (nbr_i_lo >= 0) ierr = MPI_Send(&my_record, 1, mpi_record_type, nbr_i_lo, 0, comm2D); // top
            if (nbr_i_hi >= 0) ierr = MPI_Send(&my_record, 1, mpi_record_type, nbr_i_hi, 0, comm2D); // bottom
            if (nbr_j_lo >= 0) ierr = MPI_Send(&my_record, 1, mpi_record_type, nbr_j_lo, 0, comm2D); // left
            if (nbr_j_hi >= 0) ierr = MPI_Send(&my_record, 1, mpi_record_type, nbr_j_hi, 0, comm2D); // right

            // if the generated record magnitude is greater than 3
            if (my_record.magnitude > 3.0) {
                // These may need tweaking.
                const float threshold_distance = 5000.0;
                const float threshold_magnitude = 5.0;
                const float threshold_depth = 5.0;
                // send request to adjacent neighbours to acquire their readings and compare
                Record top_record, bottom_record, left_record, right_record = {};
                if (nbr_j_lo >= 0) ierr = MPI_Recv(&left_record, 1, mpi_record_type, nbr_j_lo, 0, comm2D, &status);
                if (nbr_j_hi >= 0) ierr = MPI_Recv(&right_record, 1, mpi_record_type, nbr_j_hi, 0, comm2D, &status);
                if (nbr_i_hi >= 0) ierr = MPI_Recv(&bottom_record, 1, mpi_record_type, nbr_i_hi, 0, comm2D, &status);
                if (nbr_i_lo >= 0) ierr = MPI_Recv(&top_record, 1, mpi_record_type, nbr_i_lo, 0, comm2D, &status);
                
                int neighbours_matching=0; // if neighbouring records are within threshold, increment
                
                // recv from neighbours
                printf("rank (%d)(2) magnitude over 3 (%f).\n", my_rank, my_record.magnitude);
                printf("rank (%d)(3) Cart rank: %d. Coord: (%d, %d). Left: %d. Right: %d. Top: %d. Bottom: %d\n", my_rank,
                my_cart_rank, coord[0], coord[1], nbr_j_lo, nbr_j_hi, nbr_i_lo, nbr_i_hi); 
                if (nbr_i_lo >= 0) {
                    printf("rank (%d)(4) top record: ", my_rank);
                    PrintRecord(&top_record);
                    // compare readings TODO: break out into a function
                    // compute absolute difference of latitude and longitude between records
                    float my_lat, my_lon, nbr_lat, nbr_lon;
                    my_lat = my_record.latitude;
                    my_lon = my_record.longitude;
                    nbr_lat = top_record.latitude;
                    nbr_lon = top_record.longitude;
                    double abs_distance;
                    abs_distance = distance(my_lat, my_lon, nbr_lat, nbr_lon);
                    printf("rank (%d) absolute difference from rank (%d): %f\n", my_rank, top_record.my_rank, abs_distance);
                    // compute absolute difference of magnitude between records
                    float my_mag, nbr_mag, delta_mag;
                    my_mag = my_record.magnitude;
                    nbr_mag = top_record.magnitude;
                    delta_mag = fabs(my_mag-nbr_mag);
                    printf("rank (%d) magnitude diff from rank (%d): %f\n", my_rank, top_record.my_rank, delta_mag);
                    // compute absolute difference of depth between records
                    float my_dep, nbr_dep, delta_dep;
                    my_dep = my_record.depth;
                    nbr_dep = top_record.depth;
                    delta_dep = fabs(my_dep-nbr_dep);
                    printf("rank (%d) depth diff from rank (%d): %f\n", my_rank, top_record.my_rank, delta_dep);
                    // if records are outside of acceptable threshold, send to base station
                    if (abs_distance < threshold_distance&&delta_mag<threshold_magnitude&&delta_dep<threshold_depth) {
                        // the two records are reasonably accurate in comparison to each other
                        neighbours_matching++;
                    }
                }
                if (nbr_i_hi >= 0) {
                    printf("rank (%d)(5) bottom record: ", my_rank);
                    PrintRecord(&bottom_record);
                    // compare readings
                    // compute absolute difference of latitude and longitude between records
                    float my_lat, my_lon, nbr_lat, nbr_lon;
                    my_lat = my_record.latitude;
                    my_lon = my_record.longitude;
                    nbr_lat = bottom_record.latitude;
                    nbr_lon = bottom_record.longitude;
                    double abs_distance;
                    abs_distance = distance(my_lat, my_lon, nbr_lat, nbr_lon);
                    printf("rank (%d) absolute difference from rank (%d): %f\n", my_rank, bottom_record.my_rank, abs_distance);
                    // compute absolute difference of magnitude between records
                    float my_mag, nbr_mag, delta_mag;
                    my_mag = my_record.magnitude;
                    nbr_mag = bottom_record.magnitude;
                    delta_mag = fabs(my_mag-nbr_mag);
                    printf("rank (%d) magnitude diff from rank (%d): %f\n", my_rank, bottom_record.my_rank, delta_mag);
                    // compute absolute difference of depth between records
                    float my_dep, nbr_dep, delta_dep;
                    my_dep = my_record.depth;
                    nbr_dep = bottom_record.depth;
                    delta_dep = fabs(my_dep-nbr_dep);
                    printf("rank (%d) depth diff from rank (%d): %f\n", my_rank, bottom_record.my_rank, delta_dep);
                    // if records are outside of acceptable threshold, send to base station
                    if (abs_distance < threshold_distance&&delta_mag<threshold_magnitude&&delta_dep<threshold_depth) {
                        // the two records are reasonably accurate in comparison to each other
                        neighbours_matching++;
                    }
                }
                if (nbr_j_lo >= 0) {
                    printf("rank (%d)(6) left record: ", my_rank);
                    PrintRecord(&left_record);
                    // compare readings
                    // compute absolute difference of latitude and longitude between records
                    float my_lat, my_lon, nbr_lat, nbr_lon;
                    my_lat = my_record.latitude;
                    my_lon = my_record.longitude;
                    nbr_lat = left_record.latitude;
                    nbr_lon = left_record.longitude;
                    double abs_distance;
                    abs_distance = distance(my_lat, my_lon, nbr_lat, nbr_lon);
                    printf("rank (%d) absolute difference from rank (%d): %f\n", my_rank, left_record.my_rank, abs_distance);
                    // compute absolute difference of magnitude between records
                    float my_mag, nbr_mag, delta_mag;
                    my_mag = my_record.magnitude;
                    nbr_mag = left_record.magnitude;
                    delta_mag = fabs(my_mag-nbr_mag);
                    printf("rank (%d) magnitude diff from rank (%d): %f\n", my_rank, left_record.my_rank, delta_mag);
                    // compute absolute difference of depth between records
                    float my_dep, nbr_dep, delta_dep;
                    my_dep = my_record.depth;
                    nbr_dep = left_record.depth;
                    delta_dep = fabs(my_dep-nbr_dep);
                    printf("rank (%d) depth diff from rank (%d): %f\n", my_rank, left_record.my_rank, delta_dep);
                    // if records are outside of acceptable threshold, send to base station
                    if (abs_distance < threshold_distance&&delta_mag<threshold_magnitude&&delta_dep<threshold_depth) {
                        // the two records are reasonably accurate in comparison to each other
                        neighbours_matching++;
                    }
                }
                if (nbr_j_hi >= 0) {
                    printf("rank (%d)(7) right record: ", my_rank);
                    PrintRecord(&right_record);
                    // compare readings
                    // compute absolute difference of latitude and longitude between records
                    float my_lat, my_lon, nbr_lat, nbr_lon;
                    my_lat = my_record.latitude;
                    my_lon = my_record.longitude;
                    nbr_lat = right_record.latitude;
                    nbr_lon = right_record.longitude;
                    double abs_distance;
                    abs_distance = distance(my_lat, my_lon, nbr_lat, nbr_lon);
                    printf("rank (%d) absolute difference from rank (%d): %f\n", my_rank, right_record.my_rank, abs_distance);
                    // compute absolute difference of magnitude between records
                    float my_mag, nbr_mag, delta_mag;
                    my_mag = my_record.magnitude;
                    nbr_mag = right_record.magnitude;
                    delta_mag = fabs(my_mag-nbr_mag);
                    printf("rank (%d) magnitude diff from rank (%d): %f\n", my_rank, right_record.my_rank, delta_mag);
                    // compute absolute difference of depth between records
                    float my_dep, nbr_dep, delta_dep;
                    my_dep = my_record.depth;
                    nbr_dep = left_record.depth;
                    delta_dep = fabs(my_dep-nbr_dep);
                    printf("rank (%d) depth diff from rank (%d): %f\n", my_rank, left_record.my_rank, delta_dep);
                    // if records are outside of acceptable threshold, send to base station
                    if (abs_distance < threshold_distance&&delta_mag<threshold_magnitude&&delta_dep<threshold_depth) {
                        // the two records are reasonably accurate in comparison to each other
                        neighbours_matching++;
                    }
                }
                // TODO: if two or more neighbours have matching records (within threshold)
                if (neighbours_matching >= 2) {
                    // TODO: send to base station
                    printf("~~~ rank(%d) should send its record to base station. (%d) records matched from neighbours ~~~\n", my_rank, neighbours_matching);
                }
                printf("rank (%d)(8) end of output\n\n", my_rank);
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
    printf("rank (%d) %d %d %d %d %d %d %f %f %f %f\n", record->my_rank,
    record->current_year, record->current_month, record->current_day,
    record->current_hour, record->current_min, record->current_sec,
    record->latitude, record->longitude, record->magnitude, record->depth);
}

// TODO: reference this correctly
// https://www.geodatasource.com/developers/c
double distance(double lat1, double lon1, double lat2, double lon2) {
  double diff_lat, diff_lon, dist;
  if ((lat1 == lat2) && (lon1 == lon2)) {
    return 0;
  }
  else {
    // Haversine formula
    double radius = 6371;

    lat1 = deg2rad(lat1);
    lon1 = deg2rad(lon1);
    lat2 = deg2rad(lat2);
    lon2 = deg2rad(lon2);

    diff_lat = lat2 - lat1;
    diff_lon = lon2 - lon1;
    dist = pow(sin(diff_lat/2), 2) + (cos(lat1) * cos(lat2) * pow(sin(diff_lon/2), 2));
    dist = 2 * radius * asin(sqrt(dist));
    }
    return (dist);
}

/*:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::*/
/*::  This function converts decimal degrees to radians             :*/
/*:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::*/
double deg2rad(double deg) {
  return (deg * M_PI / 180);
}