###############################################################################
# CS/CNS 171 Fall 2015
###############################################################################
CC = g++
FLAGS = -g -std=c++11
SOURCES = *.h main.cpp ppm_gen.o

EXENAME = main

all: $(SOURCES)
	$(CC) $(FLAGS) -o $(EXENAME) $(SOURCES)

ppm_gen.o: ppm_gen.cpp ppm_gen.h
	$(CC) $(FLAGS) -c ppm_gen.cpp

clean:
	rm -f *.o $(EXENAME)

.PHONY: all clean


