# Makefile for unsupseg project

# Compilers
CC = gcc
CXX = g++

# Compiler and linker flags
FLAGS = -Wall -O3 -lpthread -msse -std=c++11
LINKS = -lz -lm -fopenmp

# Libraries
GFTLIB = -L$(GFT_DIR)/lib -lgft
GFTFLAGS = -I$(GFT_DIR)/include

export GFT_DIR=gft


# Targets
all: unsupseg unsupseg_mergehistory mergehistory_unsupseg mergehistory_ctree

# Rule for building the GFT library
libgft:
	$(MAKE) -C $(GFT_DIR)

# Rules for building the executables
unsupseg: unsupseg.o libgft
	$(CXX) $(FLAGS) $(GFTFLAGS) unsupseg.o $(GFTLIB) -o unsupseg $(LINKS)

unsupseg.o: unsupseg.cpp
	$(CXX) $(FLAGS) $(GFTFLAGS) -c unsupseg.cpp

unsupseg_mergehistory: unsupseg_mergehistory.o libgft
	$(CXX) $(FLAGS) $(GFTFLAGS) unsupseg_mergehistory.o $(GFTLIB) -o unsupseg_mergehistory $(LINKS)

unsupseg_mergehistory.o: unsupseg_mergehistory.cpp
	$(CXX) $(FLAGS) $(GFTFLAGS) -c unsupseg_mergehistory.cpp

mergehistory_unsupseg: mergehistory_unsupseg.o libgft
	$(CXX) $(FLAGS) $(GFTFLAGS) mergehistory_unsupseg.o $(GFTLIB) -o mergehistory_unsupseg $(LINKS)

mergehistory_unsupseg.o: mergehistory_unsupseg.cpp
	$(CXX) $(FLAGS) $(GFTFLAGS) -c mergehistory_unsupseg.cpp

mergehistory_ctree: mergehistory_ctree.o libgft
	$(CXX) $(FLAGS) $(GFTFLAGS) mergehistory_ctree.o $(GFTLIB) -o mergehistory_ctree $(LINKS)

mergehistory_ctree.o: mergehistory_ctree.cpp
	$(CXX) $(FLAGS) $(GFTFLAGS) -c mergehistory_ctree.cpp

# Clean rule
clean:
	$(RM) *~ *.o unsupseg unsupseg_mergehistory mergehistory_unsupseg mergehistory_ctree
