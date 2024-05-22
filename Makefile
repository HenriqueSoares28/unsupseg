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

# Directories
BINDIR = bin
OBJDIR = obj

# Ensure directories exist
$(shell mkdir -p $(BINDIR) $(OBJDIR))

# Targets
all: $(BINDIR)/unsupseg $(BINDIR)/unsupseg_mergehistory $(BINDIR)/mergehistory_unsupseg $(BINDIR)/mergehistory_ctree

# Rule for building the GFT library
libgft:
	$(MAKE) -C $(GFT_DIR)

# Rules for building the executables
$(BINDIR)/unsupseg: $(OBJDIR)/unsupseg.o libgft
	$(CXX) $(FLAGS) $(GFTFLAGS) $(OBJDIR)/unsupseg.o $(GFTLIB) -o $(BINDIR)/unsupseg $(LINKS)

$(OBJDIR)/unsupseg.o: unsupseg.cpp
	$(CXX) $(FLAGS) $(GFTFLAGS) -c unsupseg.cpp -o $(OBJDIR)/unsupseg.o

$(BINDIR)/unsupseg_mergehistory: $(OBJDIR)/unsupseg_mergehistory.o libgft
	$(CXX) $(FLAGS) $(GFTFLAGS) $(OBJDIR)/unsupseg_mergehistory.o $(GFTLIB) -o $(BINDIR)/unsupseg_mergehistory $(LINKS)

$(OBJDIR)/unsupseg_mergehistory.o: unsupseg_mergehistory.cpp
	$(CXX) $(FLAGS) $(GFTFLAGS) -c unsupseg_mergehistory.cpp -o $(OBJDIR)/unsupseg_mergehistory.o

$(BINDIR)/mergehistory_unsupseg: $(OBJDIR)/mergehistory_unsupseg.o libgft
	$(CXX) $(FLAGS) $(GFTFLAGS) $(OBJDIR)/mergehistory_unsupseg.o $(GFTLIB) -o $(BINDIR)/mergehistory_unsupseg $(LINKS)

$(OBJDIR)/mergehistory_unsupseg.o: mergehistory_unsupseg.cpp
	$(CXX) $(FLAGS) $(GFTFLAGS) -c mergehistory_unsupseg.cpp -o $(OBJDIR)/mergehistory_unsupseg.o

$(BINDIR)/mergehistory_ctree: $(OBJDIR)/mergehistory_ctree.o libgft
	$(CXX) $(FLAGS) $(GFTFLAGS) $(OBJDIR)/mergehistory_ctree.o $(GFTLIB) -o $(BINDIR)/mergehistory_ctree $(LINKS)

$(OBJDIR)/mergehistory_ctree.o: mergehistory_ctree.cpp
	$(CXX) $(FLAGS) $(GFTFLAGS) -c mergehistory_ctree.cpp -o $(OBJDIR)/mergehistory_ctree.o

# Clean rule
clean:
	$(RM) *~ $(OBJDIR)/*.o $(BINDIR)/unsupseg $(BINDIR)/unsupseg_mergehistory $(BINDIR)/mergehistory_unsupseg $(BINDIR)/mergehistory_ctree
