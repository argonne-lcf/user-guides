#
# Definition of MACROS

CXX = mpicxx
#CXXFLAGS = -g -fiopenmp -fopenmp-targets=spir64
CXXFLAGS = -fiopenmp -fopenmp-targets=spir64_gen -Xopenmp-target-backend=spir64_gen "-device 12.60.7"
#CXXFLAGS += -Wno-openmp-mapping

CPP = cpp -P -traditional
CPPFLAGS =

LD = $(CXX)
LIB = 

BINROOT=./
EX=hello_affinity
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
