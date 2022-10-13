#include "balloon.h"

typedef struct {
    int current_year, current_month, current_day; // date variables
    int current_hour, current_min, current_sec; // time variables
    float latitude, longitude, magnitude, depth; // sensor reading variables
    int my_rank; // rank of process that created record
} Record;

void PrintRecord(Record *record);
Record GenerateRecord(int sensor_rank);

void* balloon(void *pArg) {
    clock_t TimeZero = clock();
    double deltaTime = 0;
    double secondsToDelay = 5;
    bool exit = false;
    printf("hello world");
    while (!exit) {
        deltaTime = (clock() - TimeZero) / CLOCKS_PER_SEC;
        
        // every 5 seconds
        if(deltaTime == secondsToDelay){
            // generate random records
            Record my_record = GenerateRecord(0);
            PrintRecord(&my_record);
        }
        exit = true;
    }
    return NULL;
}

int main() {
    pthread_t tid;
    // Fork
    pthread_create(&tid, NULL, balloon, NULL);

    // Join
    pthread_join(tid, NULL);
    return 0;
}

void PrintRecord(Record *record) {
    printf("rank (%d) %d %d %d %d %d %d %f %f %f %f\n", record->my_rank,
    record->current_year, record->current_month, record->current_day,
    record->current_hour, record->current_min, record->current_sec,
    record->latitude, record->longitude, record->magnitude, record->depth);
}

Record GenerateRecord(int sensor_rank) {
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
    int current_day = current_time->tm_mday;
    int current_hour = current_time->tm_hour;
    int current_min = current_time->tm_min;
    int current_sec = current_time->tm_sec;

    // create record TODO: create function
    Record my_record = {current_year, current_month, current_day,
    current_hour, current_min, current_sec, latitude, longitude, magnitude, depth, sensor_rank};

    return my_record;
}