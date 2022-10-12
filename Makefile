CC=mpicc
CFLAGS=-fopenmp -lm
EXE=./a2run
SRCS=main.c base_station.c sensor.c

default:
	$(CC) $(SRCS) -o $(EXE)  $(CFLAGS)
run:
	mpirun -np 17 --oversubscribe $(EXE) 4 4