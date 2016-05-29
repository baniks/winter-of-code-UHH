##############################################################################
# Makefile for compiling mainWOC.cpp
# Things to change:
#	1. CCFLAGS- set of the path of the file omp.h using -I flag
#	2. Target mainWOC: set the lib path of openmp using -L flag
#	3. Rules: set the lib path of openmp using -L flag
##############################################################################

CC = gcc
CPP = g++
LINK = g++
LINKFLAGS =	-g -O
USRLIB = 
SYSLIB =
CCFLAGS = -g -O -fopenmp -std=c++11 -I/usr/local/include/
CFLAGS = `pkg-config --cflags opencv`
LIBS = `pkg-config --libs opencv`
  

# Targets

all:	mainWOC

mainWOC:	mainWOC.o
	g++ $(LINKFLAGS) mainWOC.o $(SYSLIB) $(CCFLAGS) $(CFLAGS) $(LIBS) -L/usr/local/lib -o mainWOC

clean:
	rm *.o mainWOC

# Rules

%.o:%.cpp
	g++ $(CFLAGS) $(CCFLAGS) -L/usr/local/lib  -c $<


