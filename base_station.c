#include "base_station.h"
#include "balloon.h"
#include "sensor.h"

int base_station(MPI_Comm master_comm, MPI_Comm slave_comm, int num_iterations) {
    int i;
    int size,sensors_alive;
    int flag = 0;
    
    MPI_Request request;
    MPI_Status status;
    MPI_Comm_size(slave_comm, &size);

    // create custom MPI datatype for Record
    const int nitems = 11;
    int blocklengths[11] = {1,1,1,1,1,1,1,1,1,1,1};;
    MPI_Datatype types[11] = {MPI_INT, MPI_INT, MPI_INT, MPI_INT, MPI_INT, MPI_INT, MPI_FLOAT, MPI_FLOAT, MPI_FLOAT, MPI_FLOAT, MPI_INT};
    MPI_Datatype mpi_record_type;
    
    MPI_Aint offsets[11];
    offsets[0] = offsetof(Record, current_year);
    offsets[1] = offsetof(Record, current_month);
    offsets[2] = offsetof(Record, current_day);
    offsets[3] = offsetof(Record, current_hour);
    offsets[4] = offsetof(Record, current_min);
    offsets[5] = offsetof(Record, current_sec);
    offsets[6] = offsetof(Record, latitude);
    offsets[7] = offsetof(Record, longitude);
    offsets[8] = offsetof(Record, magnitude);
    offsets[9] = offsetof(Record, depth);
    offsets[10] = offsetof(Record, my_rank);

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
    Record reporting_node;
    int rep_node;
    int iters = 0;
    MPI_Irecv(&reporting_node, 1, mpi_record_type, MPI_ANY_SOURCE, MPI_ANY_TAG, master_comm, &request);
    while (sensors_alive > 0) {
        MPI_Test(&request, &flag, &status);
        if (flag) {
            //MPI_Recv(&reporting_node,1,mpi_record_type, MPI_ANY_SOURCE,MPI_ANY_TAG,MPI_COMM_WORLD, &status);
            switch (status.MPI_TAG) {
                case MSG_EXIT:
                    sensors_alive--;
                break;

                case MSG_SEND:
                    fprintf(fp, "---------------------------------------------------------------------------------------------------------\n");
                    fprintf(fp, "Iteration: \n");
                    fprintf(fp, "Logged time: \n");
                    fprintf(fp, "Alert reported time: \n");
                    fprintf(fp, "Alert type: \n");
                    fprintf(fp, "\nReporting Node          Seismic Coord                        Magnitude                   IPv4\n");
                    //fprintf(fp,"        %d", rep_node);
                    fprintf(fp, "        %d                    (%f,%f)                              %f                          \n", reporting_node.my_rank, reporting_node.latitude, reporting_node.longitude, reporting_node.magnitude);
                    fprintf(fp, "\nAdjacent Nodes          Seismic Coord     Diff(Coord,km)     Magnitude     Diff(Mag)     IPv4\n");
                    for (int i=0; i<4;i++) {
                        fprintf(fp, "blah blah blah\n");
                    }
                    fprintf(fp, "\nBalloon seismic reporting time: \n");
                    fprintf(fp, "Balloon seismic reporting Coord: \n");
                    fprintf(fp, "Balloon seismic reporting Coord Diff with Reporting Node (km): \n");
                    fprintf(fp, "Balloon seismic reporting Magnitude: \n");
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
                        fclose(fp);
                    }
                break;
            }
            MPI_Irecv(&reporting_node, 1, mpi_record_type, MPI_ANY_SOURCE, MPI_ANY_TAG, master_comm, &request);
        }
    }
    fclose(fp);

    // Join
    pthread_join(tid, NULL);
    return 0;
}
