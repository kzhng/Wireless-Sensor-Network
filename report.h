#ifndef REPORT_H
#define REPORT_H

#include "record.h"

typedef struct {
        time_t alert_time;
        int nbr_match;
        Record rep_rec;
        Record top_rec;
        Record left_rec;
        Record right_rec;
        Record bot_rec;
} Report;

#endif