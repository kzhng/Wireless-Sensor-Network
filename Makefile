CC=mpicc
CFLAGS=-fopenmp -lm
EXE=./a2run
SRCS=main.c base_station.c sensor.c

default:
	$(CC) $(CFLAGS) $(SRCS) -o $(EXE)
run:
	mpirun -np 12 --oversubscribe $(EXE) 4 3