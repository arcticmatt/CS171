###############################################################################
# CS/CNS 171 Fall 2015
###############################################################################
CC = g++
FLAGS = -g -std=c++11
INCLUDE = -I../
SOURCES = *.h main.cpp file_reader.o

EXENAME = main

all: $(SOURCES)
	$(CC) $(FLAGS) -o $(EXENAME) $(INCLUDE) $(SOURCES) ../Part1/obj_reader.o ../Part2/matrix_math.o

file_reader.o: file_reader.cpp file_reader.h
	$(CC) $(FLAGS) -c $(INCLUDE) file_reader.cpp

clean:
	rm -f *.o $(EXENAME)

.PHONY: all clean


