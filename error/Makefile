CXX = g++
CC = gcc
CFLAGS =  -g
CXXFLAGS =  -D__STDC_LIMIT_MACROS

Parser: error.o 

OUTPUTDIR = ./bin/
MKDIR = mkdir -p $(OUTPUTDIR)
OBJECTC = error

CSOURCES = error.cpp

CXXSOURCES = error.cpp

all: server

server: 
	$(MKDIR)
	$(CC) -o $(OUTPUTDIR)$(OBJECTC) $(CSOURCES) $(CFLAGS)
	$(CXX) -o $(OUTPUTDIR)$(OBJECTC) $(CXXSOURCES) $(OUTPUTDIR)server

Parser: error.o	
	g++ -o $(OBJECTC) error.o -lm

.PHONY: clean
clean:
	$(RM) -rf $(OUTPUTDIR)
	$(RM) ./*.gc??
	$(RM) ./*.o
	$(RM) $(OBJECTC)

