CC=gcc
CFLAGS=-Wall -I. -lpthread -std=gnu11
EXEC=merge_sort.out

all: $(EXEC)

%.o: %.c
	$(CC) -c $^ $(CFLAGS)

$(EXEC): merge_sort.o
	$(CC) -o $@ $^ $(CFLAGS)

test:
	$(CC) -o test.o test.c $(CFLAGS)
	./test.o

mpi:
	mpicc merge_sort.c -lm

clean:
	rm *.o *.out
