###############################################################################
OBJ = acc.exe
MainFile = acc_main.cc 
###############################################################################

SourceFile := $(wildcard $(shell pwd)/src/*.cc) 
IncludeFile := $(wildcard $(shell pwd)/include/*.hh) 

###############################################################################

ROOTCFLAGS  = $(shell root-config --cflags)
ROOTLIBS    = $(shell root-config --libs)
ROOTGLIBS = $(shell root-config --glibs)

GXX = g++ -std=c++0x
DIR_INC = -I$(ROOTSYS)/include -I$(shell pwd)/include -I$(TARTSYS)/include
CFLAGS = -Wall -O $(DIR_INC) -L$(TARTSYS)/lib -lTMVA -lMathCore -lMathMore
#-lXMLParser -lanacore -D_LARGEFILE_SOURCE -D_FILE_OFFSET_BITS=64
###############################################################################

all:$(OBJ)
$(OBJ): $(MainFile) $(SourceFile) 
	$(GXX) $(CFLAGS) $(ROOTCFLAGS) $(ROOTLIBS) $(ROOTGLIBS) -o $@ $(MainFile) $(SourceFile) 
	@echo "================================"
	@echo "Compile $@ done !"
	@echo "================================"
	
clean:
	rm -f *.o *.d $(OBJ)
###############################################################################
