#Mac-Users(OS X 10.7 or above): Uncomment the line below to ensure gcc is used instead of clang
CC = gcc-4.9
CFLAGS = -std=c11 -O1
LDLIBS = -lpthread
SOURCES = Main.c In_Out.c
HEADERS = In_Out.h posixThreads.h openMP.h

all: Main MPI

Main: $(SOURCES) $(HEADERS) matrix.o
	$(CC) -o $@ Main.c In_Out.c matrix.o $(LDLIBS) $(CFLAGS) -fopenmp

MPI: messagePassing.c messagePassing.h In_Out.c In_Out.h matrix.o
	mpicc -o $@ messagePassing.c In_Out.c matrix.o $(CFLAGS)

matrix.o: matrix.c matrix.h
	$(CC) -c -o $@ $< $(CFLAGS)

clean:
	$(RM) *.o performance.txt results.txt Main MPI
