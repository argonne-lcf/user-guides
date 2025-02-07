#
# Definition of MACROS

CXX = mpicxx
CXXFLAGS = -g -O3 -std=c++0x
CPP = cpp -P -traditional
CPPFLAGS =

LD = $(CXX)
LIB = -lOpenCL

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
	$(LD) -o $@ $(CXXFLAGS) $(CPPFLAGS) $(OBJ) $(LIB)

####################################################################

%.o: %.cpp
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
