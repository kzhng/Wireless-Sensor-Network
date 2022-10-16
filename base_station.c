#include "base_station.h"
#include "balloon.h"
#include "sensor.h"
#include "record.h"

Record balloon_readings[BALLOON_READINGS_SIZE];
int num_readings = 0;

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
    // Fork
    pthread_create(&tid, NULL, balloon, NULL);
    
    time_t log_timing;
    struct tm* logging_time;

    clock_t send_time;
    double comm_time;

    time_t alert_timing;
    struct tm* alert_time;
    Report recv_report;
    int neighbours_matched;
    Record reporting_node;
    Record top_node;
    Record left_node;
    Record right_node;
    Record bot_node;
    Record balloon;
    int rep_node;
    int iters = 0;
    MPI_Irecv((void*)&recv_report, sizeof(recv_report), MPI_BYTE, MPI_ANY_SOURCE, MPI_ANY_TAG, master_comm, &request);
    while (sensors_alive > 0) {
        MPI_Test(&request, &flag, &status);
        if (flag) {
            send_time = recv_report.sending_time;
            comm_time = (clock()-send_time)/CLOCKS_PER_SEC;
            switch (status.MPI_TAG) {
                case MSG_EXIT:
                    sensors_alive--;
                break;
                case MSG_SEND:
                    alert_timing = recv_report.alert_time;
                    alert_time = localtime(&alert_timing);

                    iters = recv_report.iter_num;

                    reporting_node = recv_report.rep_rec;
                    top_node = recv_report.top_rec;
                    left_node = recv_report.left_rec;
                    right_node = recv_report.right_rec;
                    bot_node = recv_report.bot_rec;
                    balloon = balloon_readings[num_readings-1];

                    log_timing = time(NULL);
                    logging_time = localtime(&log_timing);
                    fprintf(fp, "---------------------------------------------------------------------------------------------------------\n");
                    fprintf(fp, "Iteration: %d\n", iters);
                    fprintf(fp, "Logged time: %s %d-%02d-%02d %02d:%02d:%02d\n", getWDay(logging_time->tm_wday), logging_time->tm_year + 1900, logging_time->tm_mon + 1, logging_time->tm_mday, logging_time->tm_hour, logging_time->tm_min, logging_time->tm_sec);
                    fprintf(fp, "Alert reported time: %s %d-%02d-%02d %02d:%02d:%02d\n", getWDay(alert_time->tm_wday), alert_time->tm_year + 1900, alert_time->tm_mon + 1, alert_time->tm_mday, alert_time->tm_hour, alert_time->tm_min, alert_time->tm_sec);

                    fprintf(fp, "Alert type: \n");
                    fprintf(fp, "\nReporting Node                Seismic Coord                         Magnitude                   IPv4\n");
                    fprintf(fp, "   %d(%d,%d)                    (%.2f,%.2f)                           %.2f                          \n", reporting_node.my_rank, reporting_node.x_coord, reporting_node.y_coord, reporting_node.latitude, reporting_node.longitude, reporting_node.magnitude);
                    fprintf(fp, "\nAdjacent Nodes                Seismic Coord     Diff(Coord,km)      Magnitude     Diff(Mag)     IPv4\n");
                    fprintf(fp, "   %d(%d,%d)                    (%.2f,%.2f)                           %.2f                          \n", top_node.my_rank, top_node.x_coord, top_node.y_coord, top_node.latitude, top_node.longitude, top_node.magnitude);
                    fprintf(fp, "   %d(%d,%d)                    (%.2f,%.2f)                           %.2f                          \n", left_node.my_rank, left_node.x_coord, left_node.y_coord, left_node.latitude, left_node.longitude, left_node.magnitude);
                    fprintf(fp, "   %d(%d,%d)                    (%.2f,%.2f)                           %.2f                          \n", right_node.my_rank, right_node.x_coord, right_node.y_coord, right_node.latitude, right_node.longitude, right_node.magnitude);
                    fprintf(fp, "   %d(%d,%d)                    (%.2f,%.2f)                           %.2f                          \n", bot_node.my_rank, bot_node.x_coord, bot_node.y_coord, bot_node.latitude, bot_node.longitude, bot_node.magnitude);
                    fprintf(fp, "\nBalloon seismic reporting time: %s %d-%02d-%02d %02d:%02d:%02d\n", getWDay(balloon.current_day), balloon.current_year, balloon.current_month, balloon.current_date, balloon.current_hour, balloon.current_min, balloon.current_sec);
                    fprintf(fp, "Balloon seismic reporting Coord: (%.2f,%.2f)\n", balloon.latitude, balloon.longitude);
                    fprintf(fp, "Balloon seismic reporting Coord Diff with Reporting Node (km): \n");
                    fprintf(fp, "Balloon seismic reporting Magnitude: %.2f\n", balloon.magnitude);
                    fprintf(fp, "Balloon seismic reporting Magnitude Diff with Reporting Node: \n");
                    
                    fprintf(fp, "\nCommunication time (seconds): %f\n", comm_time);
                    fprintf(fp, "Total messages sent between reporting node and base station: \n");
                    fprintf(fp, "Number of adjacent matches to reporting node: \n");
                    fprintf(fp, "Coordinate difference threshold (km): \n");
                    fprintf(fp, "Magnitude difference threshold: \n");
                    fprintf(fp, "Earthquake magnitude threshold: \n");
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
