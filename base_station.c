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

    // create custom MPI datatype for Record
    const int nitems = 14;
    int blocklengths[14] = {1,1,1,1,1,1,1,1,1,1,1,1,1};
    MPI_Datatype types[14] = {MPI_INT, MPI_INT, MPI_INT, MPI_INT, MPI_INT, MPI_INT, MPI_INT, MPI_FLOAT, MPI_FLOAT, MPI_FLOAT, MPI_FLOAT, MPI_INT, MPI_INT, MPI_INT};
    MPI_Datatype mpi_record_type;
    
    MPI_Aint offsets[14];
    offsets[0] = offsetof(Record, current_year);
    offsets[1] = offsetof(Record, current_month);
    offsets[2] = offsetof(Record, current_date);
    offsets[3] = offsetof(Record, current_day);
    offsets[4] = offsetof(Record, current_hour);
    offsets[5] = offsetof(Record, current_min);
    offsets[6] = offsetof(Record, current_sec);
    offsets[7] = offsetof(Record, latitude);
    offsets[8] = offsetof(Record, longitude);
    offsets[9] = offsetof(Record, magnitude);
    offsets[10] = offsetof(Record, depth);
    offsets[11] = offsetof(Record, my_rank);
    offsets[12] = offsetof(Record, x_coord);
    offsets[13] = offsetof(Record, y_coord);

    MPI_Type_create_struct(nitems, blocklengths, offsets, types, &mpi_record_type);
    MPI_Type_commit(&mpi_record_type);

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
    Report recv_report;
    time_t logging_time;
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
            //MPI_Recv(&reporting_node,1,mpi_record_type, MPI_ANY_SOURCE,MPI_ANY_TAG,MPI_COMM_WORLD, &status);
            switch (status.MPI_TAG) {
                case MSG_EXIT:
                    sensors_alive--;
                break;
                case MSG_SEND:
                    logging_time = recv_report.log_time;
                    reporting_node = recv_report.rep_rec;
                    top_node = recv_report.top_rec;
                    left_node = recv_report.left_rec;
                    right_node = recv_report.right_rec;
                    bot_node = recv_report.bot_rec;
                    balloon = balloon_readings[num_readings-1];
                    fprintf(fp, "---------------------------------------------------------------------------------------------------------\n");
                    fprintf(fp, "Iteration: \n");
                    fprintf(fp, "Logged time: \n");
                    fprintf(fp, "Alert reported time: \n");
                    fprintf(fp, "Alert type: \n");
                    fprintf(fp, "\nReporting Node                Seismic Coord                         Magnitude                   IPv4\n");
                    fprintf(fp, "   %d(%d,%d)                    (%.2f,%.2f)                           %.2f                          \n", reporting_node.my_rank, reporting_node.x_coord, reporting_node.y_coord, reporting_node.latitude, reporting_node.longitude, reporting_node.magnitude);
                    fprintf(fp, "\nAdjacent Nodes                Seismic Coord     Diff(Coord,km)      Magnitude     Diff(Mag)     IPv4\n");
                    fprintf(fp, "   %d(%d,%d)                    (%.2f,%.2f)                           %.2f                          \n", top_node.my_rank, top_node.x_coord, top_node.y_coord, top_node.latitude, top_node.longitude, top_node.magnitude);
                    fprintf(fp, "   %d(%d,%d)                    (%.2f,%.2f)                           %.2f                          \n", left_node.my_rank, left_node.x_coord, left_node.y_coord, left_node.latitude, left_node.longitude, left_node.magnitude);
                    fprintf(fp, "   %d(%d,%d)                    (%.2f,%.2f)                           %.2f                          \n", right_node.my_rank, right_node.x_coord, right_node.y_coord, right_node.latitude, right_node.longitude, right_node.magnitude);
                    fprintf(fp, "   %d(%d,%d)                    (%.2f,%.2f)                           %.2f                          \n", bot_node.my_rank, bot_node.x_coord, bot_node.y_coord, bot_node.latitude, bot_node.longitude, bot_node.magnitude);
                    fprintf(fp, "\nBalloon seismic reporting time: %s %d-%d-%d %02d:%02d:%02d\n", getWDay(balloon.current_day), balloon.current_year, balloon.current_month, balloon.current_date, balloon.current_hour, balloon.current_min, balloon.current_sec);
                    fprintf(fp, "Balloon seismic reporting Coord: (%.2f,%.2f)\n", balloon.latitude, balloon.longitude);
                    fprintf(fp, "Balloon seismic reporting Coord Diff with Reporting Node (km): \n");
                    fprintf(fp, "Balloon seismic reporting Magnitude: %.2f\n", balloon.magnitude);
                    fprintf(fp, "Balloon seismic reporting Magnitude Diff with Reporting Node: \n");
                    
                    fprintf(fp, "\nCommunication time (seconds): \n");
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
