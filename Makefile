all: unsupseg unsupseg_mergehistory mergehistory_unsupseg mergehistory_ctree

#Compiladores
CC=gcc
CXX=g++

FLAGS= -Wall -O3 -lpthread -msse
#-march=native 

LINKS= -lz -lm -fopenmp

#Bibliotecas
GFTLIB  = -L$(GFT_DIR)/lib -lgft
GFTFLAGS  = -I$(GFT_DIR)/include


#Rules
libgft:
	$(MAKE) -C $(GFT_DIR)

unsupseg: unsupseg.cpp libgft
	$(CXX) $(FLAGS) $(GFTFLAGS) \
	unsupseg.cpp $(GFTLIB) -o unsupseg $(LINKS)

unsupseg_mergehistory: unsupseg_mergehistory.cpp libgft
	$(CXX) $(FLAGS) $(GFTFLAGS) \
	unsupseg_mergehistory.cpp $(GFTLIB) -o unsupseg_mergehistory $(LINKS)

mergehistory_unsupseg: mergehistory_unsupseg.cpp libgft
	$(CXX) $(FLAGS) $(GFTFLAGS) \
	mergehistory_unsupseg.cpp $(GFTLIB) -o mergehistory_unsupseg $(LINKS)

mergehistory_ctree: mergehistory_ctree.cpp libgft
	$(CXX) $(FLAGS) $(GFTFLAGS) \
	mergehistory_ctree.cpp $(GFTLIB) -o mergehistory_ctree $(LINKS)

clean:
	$(RM) *~ *.o unsupseg unsupseg_mergehistory mergehistory_unsupseg mergehistory_ctree


