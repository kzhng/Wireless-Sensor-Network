#include "base_station.h"
#include "balloon.h"
#include "sensor.h"
#include "record.h"

Record balloon_readings[BALLOON_READINGS_SIZE];
int num_readings = 0;
pthread_mutex_t gMutex;

int base_station(MPI_Comm master_comm, MPI_Comm slave_comm, int num_iterations) {
    int i;
    int size,sensors_alive;
    int flag = 0;
    int nbrs_match;
    
    MPI_Request request;
    MPI_Status status;
    MPI_Comm_size(slave_comm, &size);

    sensors_alive = size;
    
    printf("NUMBER OF ITERATIONS SPECIFIED BY USER: %d\n", num_iterations);

    // log file for base station
    FILE *fp;
    fp = fopen("report_log.txt", "w+");
    if (fp == NULL) {
                perror("File error.");
                MPI_Finalize();
                return EXIT_FAILURE;
    }
    pthread_t tid;
    pthread_mutex_init(&gMutex, NULL);
    // Fork
    pthread_create(&tid, NULL, balloon, NULL);
    
    time_t log_timing;
    struct tm* logging_time;

    clock_t send_time;
    clock_t recv_time;
    long double comm_time;

    time_t alert_timing;
    struct tm* alert_time;
    Report recv_report;
    int neighbours_matched;
    Record reporting_node;
    Record nbr_node;
    Record balloon;

    bool valid_nbrs[4];

    int rep_node;
    int top_node;
    int left_node;
    int right_node;
    int bot_node;
    int iters = 0;
    int j,nbr_valid;
    MPI_Irecv((void*)&recv_report, sizeof(recv_report), MPI_BYTE, MPI_ANY_SOURCE, MPI_ANY_TAG, master_comm, &request);
    while (sensors_alive > 0) {
        MPI_Test(&request, &flag, &status);
        if (flag) {
            switch (status.MPI_TAG) {
                case MSG_EXIT:
                    sensors_alive--;
                break;
                case MSG_SEND:
                    send_time = recv_report.sending_time;
                    recv_time = clock();
                    recv_time = recv_time-send_time;
                    comm_time = ((long double)recv_time)/((long double)CLOCKS_PER_SEC);
                    comm_time = fabs(comm_time);

                    alert_timing = recv_report.alert_time;
                    alert_time = localtime(&alert_timing);

                    iters = recv_report.iter_num;
                    reporting_node = recv_report.rep_rec;
                    neighbours_matched = recv_report.nbr_match;
                    top_node = recv_report.nbr_top;
                    left_node = recv_report.nbr_left;
                    right_node = recv_report.nbr_right;
                    bot_node = recv_report.nbr_bot;

                    balloon = balloon_readings[num_readings-1];

                    float dist_diff, mag_diff, depth_diff;
                    CompareRecords(&reporting_node, &balloon, &reporting_node.my_rank, &dist_diff, &mag_diff, &depth_diff);

                    log_timing = time(NULL);
                    logging_time = localtime(&log_timing);
                    fprintf(fp, "---------------------------------------------------------------------------------------------------------\n");
                    fprintf(fp, "Iteration: %d\n", iters);
                    fprintf(fp, "Logged time: %s %d-%02d-%02d %02d:%02d:%02d\n", getWDay(logging_time->tm_wday), logging_time->tm_year + 1900, logging_time->tm_mon + 1, logging_time->tm_mday, logging_time->tm_hour, logging_time->tm_min, logging_time->tm_sec);
                    fprintf(fp, "Alert reported time: %s %d-%02d-%02d %02d:%02d:%02d\n", getWDay(alert_time->tm_wday), alert_time->tm_year + 1900, alert_time->tm_mon + 1, alert_time->tm_mday, alert_time->tm_hour, alert_time->tm_min, alert_time->tm_sec);

                    fprintf(fp, "Alert type: \n");
                    fprintf(fp, "\nReporting Node                Seismic Coord                         Magnitude                   Depth                        IPv4\n");
                    fprintf(fp, "   %d(%d,%d)                    (%.2f,%.2f)                           %.2f                        %.2f\n", reporting_node.my_rank, reporting_node.x_coord, reporting_node.y_coord, reporting_node.latitude, reporting_node.longitude, reporting_node.magnitude, reporting_node.depth);
                    fprintf(fp, "\nAdjacent Nodes                Seismic Coord     Diff(Coord,km)      Magnitude     Diff(Mag)     Depth     Diff(Depth,km)     IPv4\n");

                    float nbr_distdiff=0, nbr_magdiff=0, nbr_depthdiff=0;
                    if (top_node >=0) {
                        nbr_node = recv_report.top_rec;
                        CompareRecords(&reporting_node, &nbr_node, &reporting_node.my_rank, &nbr_distdiff, &nbr_magdiff, &nbr_depthdiff);
                        fprintf(fp, "   %d(%d,%d)                    (%.2f,%.2f)        %.2f             %.2f         %.2f         %.2f           %.2f\n", nbr_node.my_rank, nbr_node.x_coord, nbr_node.y_coord, nbr_node.latitude, nbr_node.longitude, nbr_distdiff, nbr_node.magnitude, nbr_magdiff, nbr_node.depth, nbr_depthdiff);
                    }

                    if (left_node >=0) {
                        nbr_node = recv_report.left_rec;
                        CompareRecords(&reporting_node, &nbr_node, &reporting_node.my_rank, &nbr_distdiff, &nbr_magdiff, &nbr_depthdiff);
                        fprintf(fp, "   %d(%d,%d)                    (%.2f,%.2f)        %.2f             %.2f         %.2f         %.2f           %.2f\n", nbr_node.my_rank, nbr_node.x_coord, nbr_node.y_coord, nbr_node.latitude, nbr_node.longitude, nbr_distdiff, nbr_node.magnitude, nbr_magdiff, nbr_node.depth, nbr_depthdiff);
                    }

                    if (right_node >=0) {
                        nbr_node = recv_report.right_rec;
                        CompareRecords(&reporting_node, &nbr_node, &reporting_node.my_rank, &nbr_distdiff, &nbr_magdiff, &nbr_depthdiff);
                        fprintf(fp, "   %d(%d,%d)                    (%.2f,%.2f)        %.2f             %.2f         %.2f         %.2f           %.2f\n", nbr_node.my_rank, nbr_node.x_coord, nbr_node.y_coord, nbr_node.latitude, nbr_node.longitude, nbr_distdiff, nbr_node.magnitude, nbr_magdiff, nbr_node.depth, nbr_depthdiff);
                    }

                    if (bot_node >=0) {
                        nbr_node = recv_report.bot_rec;
                        CompareRecords(&reporting_node, &nbr_node, &reporting_node.my_rank, &nbr_distdiff, &nbr_magdiff, &nbr_depthdiff);
                        fprintf(fp, "   %d(%d,%d)                    (%.2f,%.2f)        %.2f             %.2f         %.2f         %.2f           %.2f\n", nbr_node.my_rank, nbr_node.x_coord, nbr_node.y_coord, nbr_node.latitude, nbr_node.longitude, nbr_distdiff, nbr_node.magnitude, nbr_magdiff, nbr_node.depth, nbr_depthdiff);
                    }
                    
                    fprintf(fp, "\nBalloon seismic reporting time: %s %d-%02d-%02d %02d:%02d:%02d\n", getWDay(balloon.current_day), balloon.current_year, balloon.current_month, balloon.current_date, balloon.current_hour, balloon.current_min, balloon.current_sec);
                    fprintf(fp, "Balloon seismic reporting Coord: (%.2f,%.2f)\n", balloon.latitude, balloon.longitude);
                    fprintf(fp, "Balloon seismic reporting Coord Diff with Reporting Node (km): %.2f\n", dist_diff);
                    fprintf(fp, "Balloon seismic reporting Magnitude: %.2f\n", balloon.magnitude);
                    fprintf(fp, "Balloon seismic reporting Magnitude Diff with Reporting Node: %.2f\n", mag_diff);
                    fprintf(fp, "Balloon seismic reporting Depth (km): %.2f\n", balloon.depth);
                    fprintf(fp, "Balloon seismic reporting Depth Diff with Reporting Node: %.2f\n", depth_diff);

                    fprintf(fp, "\nCommunication time (seconds): %Lf\n", comm_time);
                    fprintf(fp, "Total messages sent between reporting node and base station: \n");
                    fprintf(fp, "Number of adjacent matches to reporting node: %d\n", neighbours_matched);
                    fprintf(fp, "Coordinate difference threshold (km): 200\n");
                    fprintf(fp, "Magnitude difference threshold: 2.5\n");
                    fprintf(fp, "Earthquake magnitude threshold: 3.0\n");
                    fprintf(fp, "Depth difference threshold (km): 2\n");
                    fprintf(fp, "---------------------------------------------------------------------------------------------------------\n");
                    iters++;
                    if (iters>3) {
                        sensors_alive = 0;
                        fclose(fp);
                        break;
                    }
                break;
            }
            MPI_Irecv((void*)&recv_report, sizeof(recv_report), MPI_BYTE, MPI_ANY_SOURCE, MPI_ANY_TAG, master_comm, &request);
        }
    }
    fclose(fp);

    // Join
    pthread_join(tid, NULL);
    return 0;
}

char* getWDay(int wday) {
    switch (wday) {
        case 0:
            return "Sun";

        case 1:
            return "Mon";

        case 2:
            return "Tue";

        case 3:
            return "Wed";

        case 4:
            return "Thu";

        case 5:
            return "Fri";

        case 6:
            return "Sat";
    }
}
