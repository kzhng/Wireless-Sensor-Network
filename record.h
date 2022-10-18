#ifndef RECORD_H
#define RECORD_H

#include <stddef.h>
#include <mpi.h>
#include <stdio.h>
#include <math.h>
#include <stddef.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#define ORIGIN_LATITUDE -37.8136 // Melbourne CBD latitude
#define ORIGIN_LONGITUDE 144.9631 // Melbourne CBD longitude
#define MIN_MAGNITUDE 2 // min earthquake magnitude reading
#define MAX_MAGNITUDE 12 // max earthquake magnitude reading
#define MIN_DEPTH 0 // min depth reading
#define MAX_DEPTH 5 // max depth reading
#define SENSOR_BLOCK_SIZE 0.5 // how far apart the sensors are between adjacent sensors
#define MIN_DIST_DEVIATION -0.5
#define MAX_DIST_DEVIATION 0.5
#define THRESHOLD_DIST 200 // threshold distance difference between sensor coordinates
#define THRESHOLD_MAG_DIFF 2.5 // threshold magnitude difference between readings
#define THRESHOLD_DEPTH 2 // threshold depth difference between sensor depths
#define THRESHOLD_MAG 3 //threshold earthquake magnitude that triggers a possible alert for sensors

typedef struct {
    int current_year, current_month, current_date, current_day; // date variables
    int current_hour, current_min, current_sec; // time variables
    float latitude, longitude, magnitude, depth; // sensor reading variables
    int my_rank, x_coord, y_coord; // rank of process that created record, as well as (x,y) coordinate of the sensor
} Record;

MPI_Datatype CreateRecordDatatype();

void PrintRecord(Record*);
Record GenerateRecord(int sensor_rank, int x_coordinate, int y_coordinate);
Record GenerateBalloonRecord();
int CompareRecords(Record* my_record, Record* other_record, int *sensor_rank, float *abs_distance, float *delta_mag, float *delta_dep);
int CheckRecord(Record* other_record);

double deg2rad(double);
double distance(double lat1, double lon1, double lat2, double lon2);

#endif