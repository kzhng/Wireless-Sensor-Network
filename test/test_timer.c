#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include <stdbool.h>

int main() {
    clock_t TimeZero = clock();
    double deltaTime = 0;
    double secondsToDelay = 2;
    bool exit = false;
    // TODO:change while loop exit condition
    while (!exit) {
        // get delta time in seconds
        deltaTime = (clock() - TimeZero) / CLOCKS_PER_SEC;
        
        // compare if delta time is 2 seconds
        if(deltaTime == secondsToDelay){
            // generate new random number
            int i = rand() % 100 + 1; 
            printf("Random int : %d \n", i);

            //reset the clock timers
            deltaTime = clock();
            TimeZero = clock();
        }
    }

    return EXIT_SUCCESS;
}