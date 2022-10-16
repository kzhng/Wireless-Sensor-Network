#include "sensor.h"
#include "record.h"
#include <mpi.h>
#include <pthread.h>
#include <stdlib.h>

Record my_record = {};
Record my_neighbours_records[4] = {0, 0, 0, 0}; // Recrds from my neighbours
int neighbour_count = 4;
int my_neighbours[4] = {-1,-1,-1,-1}; // int value of the process number of my neighbours


int sensor_node(MPI_Comm master_comm, MPI_Comm sensor_comm, int dims[]) {
    // MPI variables
    MPI_Request request; // comm between base station <-> sensor
    MPI_Status status;
    int sensor_size, sensor_rank; // sensor size,rank
    int master_size, master_rank; // master size, rank
    MPI_Comm_size(master_comm, &master_size);
    MPI_Comm_rank(master_comm, &master_rank);
    MPI_Comm_size(sensor_comm, &sensor_size);
    MPI_Comm_rank(sensor_comm, &sensor_rank);
    // cartesian topology variables
    int nrows, ncols;
    nrows = dims[0];
    ncols = dims[1];
    int ndims = NDIMS;
    int coord[ndims];
    int wrap_around[ndims];
    int reorder;
    int ierr;
    int my_cart_rank;
    int nbr_top, nbr_left;
    int nbr_right, nbr_bot;
    int neighbours_matching;
    MPI_Comm comm2D;
    MPI_Request request_record[neighbour_count]; // comm between sensor <-> neighbour sensor
    MPI_Status request_status[neighbour_count];

    MPI_Datatype mpi_record_type = CreateRecordDatatype();

    // create cartesian topology for processes
    MPI_Dims_create(sensor_size, ndims, dims);
    // printf("Sensor Rank: %d. Comm Size: %d: Grid Dimension = [%d x %d] \n",sensor_rank,sensor_size,dims[0],dims[1]);
    // create cartesian mapping
    wrap_around[0] = wrap_around[1] = 0;
    reorder = 1;
    ierr = 0;
    ierr = MPI_Cart_create(sensor_comm, ndims, dims, wrap_around, reorder, &comm2D);
    // TODO: Error checking for this.
    // find my coordinates in the cartesian communicator group
    MPI_Cart_coords(comm2D, sensor_rank, ndims, coord);
    // use my cartesian coordinates to find my rank in cartesian group
    MPI_Cart_rank(comm2D, coord, &my_cart_rank);
    MPI_Cart_shift( comm2D, SHIFT_ROW, DISP, &my_neighbours[TOP_NBR], &my_neighbours[BTM_NBR]); // top bottom
    MPI_Cart_shift( comm2D, SHIFT_COL, DISP, &my_neighbours[LFT_NBR], &my_neighbours[RGT_NBR]); // left right
    // printf("PW[%d]: my_cart_rank PCM[%d], my coords = (%d,%d), sensor size(%d)\n",sensor_rank, my_cart_rank, coord[0], coord[1], sensor_size);
    fflush(stdout);

    nbr_top = my_neighbours[TOP_NBR];
    nbr_left = my_neighbours[LFT_NBR];
    nbr_right = my_neighbours[RGT_NBR];
    nbr_bot = my_neighbours[BTM_NBR];

    // timer to periodically create random records
    clock_t TimeZero = clock();
    double deltaTime = 0;
    double secondsToDelay = 5;
    bool termination = false;
    srand((unsigned int)time(NULL)+sensor_rank+1);

    Report myReport;
    MPI_Status msg_check_status;
    int record_send_flag = 0;

    MPI_Comm* comm_to_send = malloc(sizeof(MPI_Comm)*3);
    pthread_t tid;
    comm_to_send[0] = sensor_comm;
    comm_to_send[1] = master_comm;
    comm_to_send[2] = comm2D;
    // send communicators to pthread
    pthread_create(&tid, NULL, sensor_msg_listener, comm_to_send);

    int iter_count = 0;

    // TODO: change loop condition
    while (!termination) {
        // get delta time in seconds
        deltaTime = (clock() - TimeZero) / CLOCKS_PER_SEC;
        
        // every 5 seconds
        if(deltaTime == secondsToDelay){
            iter_count++;
            // generate random records
            my_record = GenerateRecord(sensor_rank, coord[0], coord[1]);

            printf("rank (%d) Printing record: ", my_record.my_rank);
            PrintRecord(&my_record);
            
            // if the generated record magnitude is greater than 3
            if (my_record.magnitude > 3.0) {
                // These may need tweaking.
                const float threshold_distance = 5000.0;
                const float threshold_magnitude = 5.0;
                const float threshold_depth = 5.0;

                // 1. send request to each neighbours
                for (int i=0; i < neighbour_count; i++) {
                    printf("rank (%d) sending request to neighbour (%d) for their record\n", sensor_rank, my_neighbours[i]);
                    int req=1; // TODO: Check this
                    MPI_Isend(&req, 1, MPI_INT, my_neighbours[i], MSG_REQUEST, comm2D, &request_record[i]);
                }
                printf("rank (%d) printing neighbour records:", sensor_rank);
                PrintRecord(&my_neighbours_records[TOP_NBR]);
                PrintRecord(&my_neighbours_records[BTM_NBR]);
                PrintRecord(&my_neighbours_records[LFT_NBR]);
                PrintRecord(&my_neighbours_records[RGT_NBR]);

                int neighbours_matching=0; // if neighbouring records are within threshold, increment
                
                for (int i=0; i < neighbour_count; i++) {
                    if (my_neighbours[i] >= 0) {
                        float abs_distance, delta_dep, delta_mag;
                        CompareRecords(&my_record, &my_neighbours_records[i], &sensor_rank, &abs_distance, &delta_mag, &delta_dep);

                        if (abs_distance < threshold_distance&&delta_mag<threshold_magnitude&&delta_dep<threshold_depth) {
                            // the two records are reasonably accurate in comparison to each other
                            neighbours_matching++;
                        }
                    }
                }
                
                // TODO: if two or more neighbours have matching records (within threshold)
                if (neighbours_matching >= 2) {
                    printf("~~~ rank(%d) should send its record to base station. (%d) records matched from neighbours ~~~\n", sensor_rank, neighbours_matching);
                    // creating report to send to base station
                    myReport.iter_num = iter_count;
                    myReport.alert_time = time(NULL);
                    myReport.nbr_match = neighbours_matching;
                    myReport.rep_rec = my_record;
                    // TODO: Check if record is valid/rank is valid
                    myReport.nbr_top = nbr_top;
                    myReport.nbr_left = nbr_left;
                    myReport.nbr_right = nbr_right;
                    myReport.nbr_bot = nbr_bot;
                    myReport.top_rec = my_neighbours_records[TOP_NBR];
                    myReport.left_rec = my_neighbours_records[LFT_NBR];
                    myReport.right_rec = my_neighbours_records[RGT_NBR];
                    myReport.bot_rec = my_neighbours_records[BTM_NBR];
                    myReport.sending_time = clock();
                    MPI_Isend((void *)&myReport, sizeof(myReport), MPI_BYTE, master_size-1, MSG_SEND, master_comm, &request);
                }
                // printf("rank (%d)(8) end of output\n\n", sensor_rank);
            }

            //reset the clock timers
            deltaTime = clock();
            TimeZero = clock();
        }
    }

    pthread_join(tid, NULL);
    return EXIT_SUCCESS;
}

void* sensor_msg_listener(void *pArg) {
    pthread_mutex_t gMutex;
    
    MPI_Datatype mpi_record_type = CreateRecordDatatype();
    
    MPI_Status msg_status;
    MPI_Request msg_request;
    MPI_Comm* comms = (MPI_Comm*)pArg;
    MPI_Comm sensor_comm = comms[0];
    MPI_Comm master_comm = comms[1];
    MPI_Comm comm2D = comms[2];
    int sensor_rank;
    MPI_Comm_rank(sensor_comm, &sensor_rank);


    int msg_request_flag = 0;
    int req;
    bool termination = false;

    while (!termination) {
        pthread_mutex_lock(&gMutex);
        MPI_Iprobe(MPI_ANY_SOURCE, MPI_ANY_TAG, comm2D, &msg_request_flag, &msg_status); // check for message
        if (msg_request_flag) { // if flag is true, we want to send our record to requesting neighbour
            // we send our record with this tag MSG_RECORD
            if (msg_status.MPI_TAG == MSG_REQUEST) {
                printf("rank (%d) recieved MSG_REQUEST from neighbour (%d)\n", sensor_rank, msg_status.MPI_SOURCE);
                MPI_Recv(&req, 1, MPI_INT, msg_status.MPI_SOURCE, MSG_REQUEST, comm2D, MPI_STATUS_IGNORE); // recv request
                MPI_Send(&my_record, 1, mpi_record_type, msg_status.MPI_SOURCE, MSG_RECORD, comm2D); // send record
                printf("rank (%d) sending my record to neighbour (%d) with tag MSG_RECORD, my record is:\n", sensor_rank, msg_status.MPI_SOURCE);
                PrintRecord(&my_record);
            }
            if (msg_status.MPI_TAG == MSG_RECORD) {
                printf("rank (%d) recieved a record from neighbour (%d), inserting to my_neighbour_records\n", sensor_rank, msg_status.MPI_SOURCE);
                int index;
                if (msg_status.MPI_SOURCE == my_neighbours[TOP_NBR]) {
                    index = TOP_NBR;
                }
                else if (msg_status.MPI_SOURCE == my_neighbours[BTM_NBR]) {
                    index = BTM_NBR;
                }
                else if (msg_status.MPI_SOURCE == my_neighbours[LFT_NBR]) {
                    index = LFT_NBR;
                }
                else if (msg_status.MPI_SOURCE == my_neighbours[RGT_NBR]) {
                    index = RGT_NBR;
                }
                MPI_Irecv(&my_neighbours_records[index], 1, mpi_record_type, msg_status.MPI_SOURCE, MSG_RECORD, comm2D, &msg_request);
            }
        }
        pthread_mutex_unlock(&gMutex);
    }

    return EXIT_SUCCESS;
}