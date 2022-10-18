#include "base_station.h"

pthread_mutex_t gMutex;

int base_station(MPI_Comm master_comm, MPI_Comm slave_comm, int num_iterations, int nrows, int ncols) {
    int i;
    int size,sensors_alive;
    int flag = 0;
    int nbrs_match;
    float dist_diff, mag_diff, depth_diff;
    
    MPI_Request request;
    MPI_Status status;
    MPI_Comm_size(master_comm, &size);

    sensors_alive = size - 1;

    int msgs_array[size];
    memset(msgs_array, 0, size*sizeof(int));
    
    // printf("NUMBER OF ITERATIONS SPECIFIED BY USER: %d\n", num_iterations);

    // log file for base station
    FILE *fp;
    fp = fopen("report_log.txt", "w+");
    if (fp == NULL) {
                perror("File error.");
                MPI_Finalize();
                return EXIT_FAILURE;
    }
    // queue used for storing balloon readings
    Queue *balloonQueue = createQueue(BALLOON_READINGS_SIZE);
    grid_dims *dims_grid = (grid_dims *)malloc(sizeof(grid_dims));
    dims_grid->num_rows = nrows;
    dims_grid->num_cols = ncols;
    dims_grid->q = balloonQueue;

    pthread_t tid;
    pthread_mutex_init(&gMutex, NULL);
    // Fork
    pthread_create(&tid, NULL, balloon, (void *)dims_grid);
    
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
    char* top_ipv4;
    char* bot_ipv4;
    char* left_ipv4;
    char* right_ipv4;
    char* reporting_ipv4 = (char*)malloc(4); // 32bit ipv4 address
    MPI_Irecv((void*)&recv_report, sizeof(recv_report), MPI_BYTE, MPI_ANY_SOURCE, MPI_ANY_TAG, master_comm, &request);
    while (sensors_alive > 0) {
        // test to see if we get a msg from any one of the sensor nodes about an alert
        MPI_Test(&request, &flag, &status);
        if (flag) {
            switch (status.MPI_TAG) {
                case MSG_EXIT:
                    sensors_alive--;
                break;
                case MSG_SEND:
                    // receives information from sensor node and logs it into a log txt file
                    send_time = recv_report.sending_time;
                    recv_time = clock();
                    recv_time = recv_time-send_time;
                    comm_time = ((long double)recv_time)/((long double)CLOCKS_PER_SEC);
                    comm_time = fabs(comm_time);

                    alert_timing = recv_report.alert_time;
                    alert_time = localtime(&alert_timing);

                    iters = recv_report.iter_num;
                    reporting_node = recv_report.rep_rec;
                    msgs_array[reporting_node.my_rank]++;
                    neighbours_matched = recv_report.nbr_match;
                    top_node = recv_report.nbr_top;
                    left_node = recv_report.nbr_left;
                    right_node = recv_report.nbr_right;
                    bot_node = recv_report.nbr_bot;

                    // the balloon reading that is closest in abs dist to the reporting node reading location 
                    // is chosen
                    balloon = findClosestBalloon(reporting_node, balloonQueue);
                    // printf("BASE STATION PRINTING RECORD\n");
                    // PrintRecord(&balloon);

                    // top_ipv4 = recv_report.top_rec.ipv4;
                    // bot_ipv4 = recv_report.bot_rec.ipv4;
                    // left_ipv4 = recv_report.left_rec.ipv4;
                    // right_ipv4 = recv_report.right_rec.ipv4;
                    // reporting_ipv4 = reporting_node.ipv4;
                    reporting_ipv4 = GetHostDetails();

                    CompareRecords(&reporting_node, &balloon, &reporting_node.my_rank, &dist_diff, &mag_diff, &depth_diff);

                    log_timing = time(NULL);
                    logging_time = localtime(&log_timing);
                    fprintf(fp, "---------------------------------------------------------------------------------------------------------\n");
                    fprintf(fp, "Iteration: %d\n", iters);
                    fprintf(fp, "Logged time: %s %d-%02d-%02d %02d:%02d:%02d\n", getWDay(logging_time->tm_wday), logging_time->tm_year + 1900, logging_time->tm_mon + 1, logging_time->tm_mday, logging_time->tm_hour, logging_time->tm_min, logging_time->tm_sec);
                    fprintf(fp, "Alert reported time: %s %d-%02d-%02d %02d:%02d:%02d\n", getWDay(alert_time->tm_wday), alert_time->tm_year + 1900, alert_time->tm_mon + 1, alert_time->tm_mday, alert_time->tm_hour, alert_time->tm_min, alert_time->tm_sec);

                    if (dist_diff <= THRESHOLD_DIST && mag_diff <= THRESHOLD_MAG_DIFF && depth_diff <= THRESHOLD_DEPTH) {
                        fprintf(fp, "Alert type: Conclusive\n");
                    }   else {
                        fprintf(fp, "Alert type: Inconclusive\n");
                    }
                    fprintf(fp, "\nReporting Node                Seismic Coord                         Magnitude                   Depth                        IPv4\n");
                    fprintf(fp, "   %d(%d,%d)                    (%.2f,%.2f)                           %.2f                        %.2f                         %s\n", reporting_node.my_rank, reporting_node.x_coord, reporting_node.y_coord, reporting_node.latitude, reporting_node.longitude, reporting_node.magnitude, reporting_node.depth, reporting_ipv4);
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
                    fprintf(fp, "Total messages sent between reporting node and base station: %d\n", msgs_array[reporting_node.my_rank]);
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

Record findClosestBalloon(Record rep_node, Queue *q) {
    Record balloon_node;
    int i=0, max_index=1, min_loc=0;
    float min_dist = LONG_MAX;
    float abs_dist, mag_diff, depth_diff;
    max_index = q->size - 1;
    // printf("q_size %d \n", max_index);
    // printf("WHAT IS INSIDE THE QUEUE?\n");
    for (int i=0;i<max_index;i++) {
        // printf("what is i? %d\n", i);
        balloon_node = View(q, i);
        //PrintRecord(&balloon_node);
        CompareRecords(&rep_node, &balloon_node, 0, &abs_dist, &mag_diff, &depth_diff);
        if (abs_dist < min_dist) {
            abs_dist = min_dist;
            min_loc = i;
        }
    }
    return View(q,min_loc); 
}