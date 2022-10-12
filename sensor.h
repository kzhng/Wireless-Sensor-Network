#ifndef SENSOR
#define SENSOR

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

#define NDIMS 2

typedef struct {
    int current_year, current_month, current_day; // date variables
    int current_hour, current_min, current_sec; // time variables
    float latitude, longitude, magnitude, depth; // sensor reading variables
    int my_rank; // rank of process that created record
} Record;

void PrintRecord(Record*);
Record GenerateRecord(int sensor_rank);
int sensor_node(MPI_Comm master_comm, MPI_Comm sensor_comm, int dims[]);

void set_time_variables();
double deg2rad(double);
double distance(double lat1, double lon1, double lat2, double lon2);

#endif // SENSOR