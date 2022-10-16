#include "record.h"

MPI_Datatype CreateRecordDatatype() {
    // create custom MPI datatype for Record
    const int nitems = 14;
    int blocklengths[14] = {1,1,1,1,1,1,1,1,1,1,1,1,1};
    MPI_Datatype types[14] = {MPI_INT, MPI_INT, MPI_INT, MPI_INT, MPI_INT, MPI_INT, MPI_INT, MPI_FLOAT, MPI_FLOAT, MPI_FLOAT, MPI_FLOAT, MPI_INT, MPI_INT, MPI_INT};
    MPI_Datatype mpi_record_type;
    
    MPI_Aint offsets[14];
    offsets[0] = offsetof(Record, current_year);
    offsets[1] = offsetof(Record, current_month);
    offsets[2] = offsetof(Record, current_date);
    offsets[3] = offsetof(Record, current_day);
    offsets[4] = offsetof(Record, current_hour);
    offsets[5] = offsetof(Record, current_min);
    offsets[6] = offsetof(Record, current_sec);
    offsets[7] = offsetof(Record, latitude);
    offsets[8] = offsetof(Record, longitude);
    offsets[9] = offsetof(Record, magnitude);
    offsets[10] = offsetof(Record, depth);
    offsets[11] = offsetof(Record, my_rank);
    offsets[12] = offsetof(Record, x_coord);
    offsets[13] = offsetof(Record, y_coord);

    MPI_Type_create_struct(nitems, blocklengths, offsets, types, &mpi_record_type);
    MPI_Type_commit(&mpi_record_type);

    return mpi_record_type;
}

void PrintRecord(Record *record) {
    printf("rank (%d) %d %d %d %d %d %d %f %f %f %f\n", record->my_rank,
    record->current_year, record->current_month, record->current_date,
    record->current_hour, record->current_min, record->current_sec,
    record->latitude, record->longitude, record->magnitude, record->depth);
}

Record GenerateRecord(int sensor_rank,int x_coord, int y_coord) {
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
    s = time(NULL) + sensor_rank + 1;
    current_time = localtime(&s);

    int current_year = current_time->tm_year + 1900;
    int current_month = current_time->tm_mon + 1;
    int current_date = current_time->tm_mday;
    int current_day = current_time->tm_wday;
    int current_hour = current_time->tm_hour;
    int current_min = current_time->tm_min;
    int current_sec = current_time->tm_sec;

    // create record TODO: create function
    Record my_record = {current_year, current_month, current_date, current_day,
    current_hour, current_min, current_sec, latitude, longitude, magnitude, depth, sensor_rank, x_coord, y_coord};

    return my_record;
}

int CompareRecords(Record* my_record, Record* other_record, int *sensor_rank, float *abs_distance, float *delta_mag, float *delta_dep) {
    float my_lat, my_lon, nbr_lat, nbr_lon;
    my_lat = my_record->latitude;
    my_lon = my_record->longitude;
    nbr_lat = other_record->latitude;
    nbr_lon = other_record->longitude;
    *abs_distance = distance(my_lat, my_lon, nbr_lat, nbr_lon);
    // printf("rank (%d) absolute difference from rank (%d): %f\n", *sensor_rank, other_record->my_rank, *abs_distance);
    // compute absolute difference of magnitude between records
    float my_mag, nbr_mag;
    my_mag = my_record->magnitude;
    nbr_mag = other_record->magnitude;
    *delta_mag = fabs(my_mag-nbr_mag);
    // printf("rank (%d) magnitude diff from rank (%d): %f\n", *sensor_rank, other_record->my_rank, *delta_mag);
    // compute absolute difference of depth between records
    float my_dep, nbr_dep;
    my_dep = my_record->depth;
    nbr_dep = other_record->depth;
    *delta_dep = fabs(my_dep-nbr_dep);
    // printf("rank (%d) depth diff from rank (%d): %f\n", *sensor_rank, other_record->my_rank, *delta_dep);

    return 0; // TODO: Change this
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