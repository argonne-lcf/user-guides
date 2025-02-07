# Use NVIDIA compilers w/ ALCF provided OpenMPI
#
# module load nvhpc-nompi/21.3
#
# Definition of MACROS

PATH_TO_NVCC = $(shell which nvcc)
PATH_TO_NVHPC = $(shell echo ${PATH_TO_NVCC} | rev | cut -d '/' -f 4- | rev)

$(info PATH_TO_NVHPC= [${PATH_TO_NVHPC}])

CUDA = ${PATH_TO_NVHPC}/cuda

CXX = CC
CXXFLAGS = -g -O3 -std=c++0x 
CXXFLAGS += -cuda -I$(CUDA)/include
CXXFLAGS += -D_SINGLE_PRECISION

CPP = cpp -P -traditional
CPPFLAGS =

LD = $(CXX)
LIB = -L$(CUDA)/lib64 -lcudart

BINROOT=./
EX=vecadd
SHELL=/bin/sh

SRC = $(wildcard *.cpp)
INC = $(wildcard *.h)
OBJ = $(SRC:.cpp=.o)

#
# -- target : 	Dependencies
# --		Rule to create target

$(EX): 	$(OBJ)
	$(LD) -o $@ $(CXXFLAGS) $(OBJ) $(LIB)

####################################################################

$(OBJ): %.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $<

#
# -- Remove *.o and *~ from the directory
clean:
	rm -f *.o *~
#
# -- Remove *.o, *~, and executable from the directory
realclean:
	rm -f *.o *~ ./$(EX)

#
# -- Simple dependencies

$(OBJ) : $(INC)
