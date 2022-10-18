#ifndef BALLOON_H
#define BALLOON_H

#include <pthread.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

void* balloon(void *input);
float find_min_coord(float base_coord);
float find_max_coord(float base_coord, float component);

#endif // BALLOON_H