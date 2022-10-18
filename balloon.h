#ifndef BALLOON_H
#define BALLOON_H

#include <pthread.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

void* balloon(void *input);
float find_min_coord(int base_coord);
float find_max_coord(int base_coord, int component);

#endif // BALLOON_H