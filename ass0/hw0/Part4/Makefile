###############################################################################
# CS/CNS 171 Fall 2015
###############################################################################
CC = g++
FLAGS = -g -std=c++11
SOURCES = *.h *.cpp

EXENAME = ppm_gen

all: $(SOURCES)
	$(CC) $(FLAGS) -o $(EXENAME) $(INCLUDE) $(SOURCES)

clean:
	rm -f *.o $(EXENAME)

.PHONY: all clean


