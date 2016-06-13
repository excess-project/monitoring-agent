# Makefile for minimal sparse double matrix GEMM test.
#   Copyright (C) 2015  Anders Gidenstam

CC = g++ -fopenmp

OBJS = SpMatrix.o  SpTest.o SparseAccumulator.o

CFLAGS += -std=c++11 -g -O3 -Wall -I../include -DUSE_OPENMP

all: sptest

%.o:	%.cpp
	$(CC) $(CFLAGS) $(TGFLAGS) -c $*.cpp -o $*.o

sptest: $(OBJS)
	$(CC) $(CFLAGS) $(TGFLAGS) -o sptest $(OBJS) $(LDFLAGS)

clean:
	@rm -f $(OBJS) *.gch sptest
