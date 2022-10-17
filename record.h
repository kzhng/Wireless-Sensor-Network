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

double deg2rad(double);
double distance(double lat1, double lon1, double lat2, double lon2);

#endif