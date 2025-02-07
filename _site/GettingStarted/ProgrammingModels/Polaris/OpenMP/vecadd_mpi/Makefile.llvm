# Use LLVM compiler w/ ALCF provided OpenMPI
#
# module load llvm
# module load cudatoolkit-standalone
# module load mpiwrappers/cray-mpich-llvm
#
# Definition of MACROS

CXX = mpicxx
CXXFLAGS = -g -fopenmp -O3 -std=c++0x
CXXFLAGS += -D_SINGLE_PRECISION
CXXFLAGS += -fopenmp-targets=nvptx64 -D__STRICT_ANSI__ -Wno-openmp-mapping

CPP = cpp -P -traditional
CPPFLAGS =

LD = $(CXX)
LIB = 

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
