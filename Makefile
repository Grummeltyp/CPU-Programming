#Mac-Users(OS X 10.7 or above): Uncomment the line below to ensure gcc is used instead of clang
CC = mpicc
CFLAGS = -std=c11 -fopenmp
LDLIBS = -lpthread
SOURCES = Main.c In_Out.c matrix.c

all: Main

Main: Main.c In_Out.c matrix.o

matrix:
	$(CC) $(CFLAGS) -c matrix.c -o matrix.o

clean:
	$(RM) Main