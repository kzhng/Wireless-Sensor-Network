CC=mpicc
CFLAGS=-fopenmp -lm
EXE=./a2run
SRCS=main.c base_station.c sensor.c balloon.c report.c record.c

default:
	$(CC) $(SRCS) -o $(EXE)  $(CFLAGS)
run:
	mpirun -np 7 --oversubscribe $(EXE) 3 2 5