#Mac-Users(OS X 10.7 or above): Uncomment the line below to ensure gcc is used instead of clang
CC = gcc-4.9
CFLAGS = -std=c11 -fopenmp
LDLIBS = -lpthread
SOURCES = Multiplication.c In_Out.c 
HEADERS = Multiplication.h In_Out.h

all: Multiplication

Multiplication: $(SOURCES) $(HEADERS)

clean:
	$(RM) Multiplication