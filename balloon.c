#include "balloon.h"
#include "record.h"
#include "base_station.h"

extern Record balloon_readings[BALLOON_READINGS_SIZE];
extern int num_readings;

void* balloon(void *pArg) {
    pthread_mutex_t gMutex;
    clock_t TimeZero = clock();
    double deltaTime = 0;
    double secondsToDelay = 5;
    bool exit = false;
    int index = 0;
    int i;
    printf("hello world\n");
    while (!exit) {
        deltaTime = (clock() - TimeZero) / CLOCKS_PER_SEC;
        
        // every 5 seconds
        if(deltaTime == secondsToDelay){
            // generate random records
            Record my_record = GenerateRecord(-1,-1,-1);
            pthread_mutex_lock(&gMutex);
            balloon_readings[index] = my_record;
            if (num_readings < 10) {
                num_readings++;
            }
            index = (index + 1) % BALLOON_READINGS_SIZE;
            pthread_mutex_unlock(&gMutex);
            // PrintBalloonRecord(&my_record);

            // printf("\nstart check for array data\n");
            for (int i=0; i<num_readings;i++) {
                Record i_record = balloon_readings[i];
                // PrintBalloonRecord(&i_record);
            }
            // printf("end check for array data\n\n");
            //reset the clock timers
            deltaTime = clock();
            TimeZero = clock();
        }

    }
    printf("hello jupiter\n");
    return NULL;
}

/*int main() {
    pthread_t tid;
    pthread_mutex_init(&gMutex, NULL);

    // Fork
    pthread_create(&tid, NULL, balloon, NULL);

    // Join
    pthread_join(tid, NULL);
    return 0;
}*/