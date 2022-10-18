CC=mpicc
CFLAGS=-fopenmp -lm -g
EXE=./a2run
SRCS=main.c base_station.c sensor.c balloon.c data_structures.c record.c utils.c

default:
	$(CC) $(SRCS) -o $(EXE)  $(CFLAGS)
run:
	mpirun -np 21 --oversubscribe $(EXE) 4 5 5