#include <stdio.h>
#include <time.h>

int main()
{
    time_t s, val = 1;
    struct tm* current_time;
    
    s = time(NULL);
    
    current_time = localtime(&s);
    
    printf("Day of the month = %d\n",current_time->tm_mday);
    printf("Day in this year = %d\n",current_time->tm_yday);
    printf("Day in this week = %d\n",current_time->tm_wday);
    printf("Month of this year = %d\n",(current_time->tm_mon + 1));
    printf("Current year = %d\n",(current_time->tm_year + 1900));
    printf("hour:min:sec = %02d:%02d:%02d\n",
           current_time->tm_hour,
           current_time->tm_min,
           current_time->tm_sec);
    
    return 0;
}