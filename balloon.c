#include "balloon.h"

void* balloon(void *pArg) {
    clock_t TimeZero = clock();
    double deltaTime = 0;
    double secondsToDelay = 5;
    bool exit = false;
    int index = 0, num_readings = 0;
    int i;
    printf("hello world\n");
    while (!exit) {
        deltaTime = (clock() - TimeZero) / CLOCKS_PER_SEC;
        
        // every 5 seconds
        if(deltaTime == secondsToDelay){
            // generate random records
            BalloonRecord my_record = GenerateBalloonRecord(0);
            pthread_mutex_lock(&gMutex);
            balloon_readings[index] = my_record;
            if (num_readings < 10) {
                num_readings++;
            }
            index = (index + 1) % BALLOON_READINGS_SIZE;
            pthread_mutex_unlock(&gMutex);
            PrintBalloonRecord(&my_record);

            printf("\nstart check for array data\n");
            for (int i=0; i<num_readings;i++) {
                BalloonRecord i_record = balloon_readings[i];
                PrintBalloonRecord(&i_record);
            }
            printf("end check for array data\n\n");
            //reset the clock timers
            deltaTime = clock();
            TimeZero = clock();
        }

    }
    printf("hello jupiter\n");
    return NULL;
}

int main() {
    pthread_t tid;
    pthread_mutex_init(&gMutex, NULL);

    // Fork
    pthread_create(&tid, NULL, balloon, NULL);

    // Join
    pthread_join(tid, NULL);
    return 0;
}

void PrintBalloonRecord(BalloonRecord *record) {
    printf("rank (%d) %d %d %d %d %d %d %f %f %f %f\n", record->my_rank,
    record->current_year, record->current_month, record->current_day,
    record->current_hour, record->current_min, record->current_sec,
    record->latitude, record->longitude, record->magnitude, record->depth);
}

BalloonRecord GenerateBalloonRecord(int sensor_rank) {
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
    BalloonRecord my_record = {current_year, current_month, current_day,
    current_hour, current_min, current_sec, latitude, longitude, magnitude, depth, sensor_rank};

    return my_record;
}