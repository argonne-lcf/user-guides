#
# Definition of MACROS

CXX = mpicxx
CXXFLAGS = -g -O3 -std=c++0x
CXXFLAGS += -fiopenmp -fopenmp-targets=spir64
CXXFLAGS += -D_SINGLE_PRECISION

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
