###############################################################################
# CS/CNS 171 Fall 2015
###############################################################################
CC = g++
FLAGS = -g
INCLUDE = -I../
SOURCES = *.h main.cpp matrix_math.o

EXENAME = main

all: $(SOURCES)
	$(CC) $(FLAGS) -o $(EXENAME) $(INCLUDE) $(SOURCES)

matrix_math.o: matrix_math.cpp matrix_math.h
	$(CC) $(FLAGS) -c $(INCLUDE) matrix_math.cpp

clean:
	rm -f *.o $(EXENAME)

.PHONY: all clean
