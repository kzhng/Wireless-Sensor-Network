#ifndef SENSOR
#define SENSOR

#include <stddef.h>
#include <stdio.h>
#include <math.h>
#include <stddef.h>
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
#define MSG_SEND 1
#define MSG_EXIT 2


typedef struct {
    int current_year, current_month, current_day; // date variables
    int current_hour, current_min, current_sec; // time variables
    float latitude, longitude, magnitude, depth; // sensor reading variables
    int my_rank, x_coord, y_coord; // rank of process that created record, as well as (x,y) coordinate of the sensor
} Record;

typedef struct {
        clock_t log_time;
        int nbr_match;
        Record rep_rec;
        Record top_rec;
        Record left_rec;
        Record right_rec;
        Record bot_rec;
} Report;

void PrintRecord(Record*);
Record GenerateRecord(int sensor_rank, int x_coordinate, int y_coordinate);
int CompareRecords(Record* my_record, Record* other_record, int *sensor_rank, float *abs_distance, float *delta_mag, float *delta_dep);

int sensor_node(MPI_Comm master_comm, MPI_Comm sensor_comm, int dims[]);

void set_time_variables();
double deg2rad(double);
double distance(double lat1, double lon1, double lat2, double lon2);

#endif // SENSOR