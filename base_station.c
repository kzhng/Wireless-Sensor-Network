#include "base_station.h"

#define NUM_THREADS 4
#define MPI_SEND 1
#define MPI_EXIT 2

int base_station(MPI_Comm master_comm, MPI_Comm comm) {
    
    MPI_Status status;
    MPI_Comm_size()
    pthread_t tid[NUM_THREADS];
	int threadNum[NUM_THREADS];

    // log file for base station
    FILE *fp;
    fp = fopen("report_log.txt", "w");
    if (fp == NULL) {
                perror("File error.");
                MPI_Finalize();
                return EXIT_FAILURE;
    }

    // FORK
    for (int i = 0; i < NUM_THREADS; i++) {
        threadNum[i] = i;
        pthread_create(&tid[i], 0, balloon, &threadNum[i]);
    }

    // Join
	for(i = 0; i < NUM_THREADS; i++)
	{
        pthread_join(tid[i], NULL);
	}
    while (sensors_alive > 0) {
        MPI_Recv(recv_array, 10, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);

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
        fclose(fp);
    }
    

    return 0;
}
