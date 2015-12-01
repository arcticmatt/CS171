###############################################################################
# CS/CNS 171 Fall 2015
#
# This is a template Makefile for the assignment. Edit it however you find
# convenient.
###############################################################################
CC = g++
FLAGS = -g -std=c++11

# The following line is a relative directory reference that assumes the Eigen
# folder--which your program will depend on--is located one directory above the
# directory that contains this Makefile.
INCLUDE = -I../ -I/usr/X11R6/include -I/usr/include/GL -I/usr/include
LIBDIR = -L/usr/X11R6/lib -L/usr/local/lib
SOURCES = *.h *.cpp
LIBS = -lGLEW -lGL -lGLU -lglut -lm

EXENAME = keyframe

all: $(SOURCES)
	$(CC) $(FLAGS) -o $(EXENAME) $(INCLUDE) $(LIBDIR) $(SOURCES) $(LIBS)

clean:
	rm -f *.o $(EXENAME)

.PHONY: all clean

