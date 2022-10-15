#include "base_station.h"
#include "balloon.h"

#define NUM_THREADS 4
#define MSG_SEND 1
#define MSG_EXIT 2

int base_station(MPI_Comm master_comm, MPI_Comm slave_comm, int num_iterations) {
    int i;
    int size,sensors_alive;
    int *recv_array[10];
    
    MPI_Status status;
    MPI_Comm_size(slave_comm, &size);
    
    sensors_alive = size;
    
    printf("NUMBER OF ITERATIONS SPECIFIED BY USER: %d\n", num_iterations);

    // log file for base station
    FILE *fp;
    fp = fopen("report_log.txt", "w");
    if (fp == NULL) {
                perror("File error.");
                MPI_Finalize();
                return EXIT_FAILURE;
    }

    pthread_t tid;
    // Fork
    pthread_create(&tid, NULL, balloon, NULL);

    // Join
    pthread_join(tid, NULL);

    while (sensors_alive > 0) {
        MPI_Recv(&recv_array, 10, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);

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
                fprintf(fp, "blah blah\n");
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

            break;
        }
    }
    fclose(fp);

    return 0;
}
