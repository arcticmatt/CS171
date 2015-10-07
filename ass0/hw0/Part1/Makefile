###############################################################################
# CS/CNS 171 Fall 2015
###############################################################################
CC = g++
FLAGS = -g -std=c++11
INCLUDE = -I../
SOURCES = *.h main.cpp obj_reader.o

EXENAME = main

all: $(SOURCES)
	$(CC) $(FLAGS) -o $(EXENAME) $(INCLUDE) $(SOURCES)

obj_reader.o: obj_reader.cpp obj_reader.h
	$(CC) $(FLAGS) -c $(INCLUDE) obj_reader.cpp

clean:
	rm -f *.o $(EXENAME)

.PHONY: all clean
