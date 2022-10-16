#ifndef SENSOR
#define SENSOR

#include <stddef.h>
#include <stdio.h>
#include <math.h>
#include <stddef.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <mpi.h>
#include "record.h"
#include "report.h"

#define SHIFT_ROW 0
#define SHIFT_COL 1
#define DISP 1
#define UPPER 30
#define LOWER 3

#define NDIMS 2
#define MSG_SEND 1 // sensor <-> base station
#define MSG_REQUEST 2 // sending request to neighbours
#define MSG_RECORD 3 // sensor <-> neighbour sensor
#define MSG_EXIT 4 // TODO: termination

#define TOP_NBR 0
#define LFT_NBR 1
#define RGT_NBR 2
#define BTM_NBR 3




int sensor_node(MPI_Comm master_comm, MPI_Comm sensor_comm, int dims[]);

void* sensor_msg_listener(void *pArg);

#endif // SENSOR