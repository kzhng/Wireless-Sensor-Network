CC=mpicc
CFLAGS=-fopenmp -lm
EXE=./a2run
SRCS=main.c base_station.c sensor.c balloon.c

default:
	$(CC) $(SRCS) -o $(EXE)  $(CFLAGS)
run:
	mpirun -np 21 --oversubscribe $(EXE) 4 5