#ifndef BALLOON_H
#define BALLOON_H

#include <pthread.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

pthread_mutex_t gMutex;

void* balloon(void *pArg);

#endif // BALLOON_H