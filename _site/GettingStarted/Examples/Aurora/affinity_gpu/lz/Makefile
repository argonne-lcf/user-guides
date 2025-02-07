#
# Definition of MACROS

CXX = mpicxx 
#-cxx=icpx
CXXFLAGS = -g -O3 -std=c++17 
#CXXFLAGS += -fiopenmp -fopenmp-targets=spir64 -fsycl -D__STRICT_ANSI__
CXXFLAGS += -fsycl -D__STRICT_ANSI__
CXXFLAGS += -D_USE_GPU
#CXXFLAGS += -Wno-openmp-mapping

CPP = cpp -P -traditional
CPPFLAGS =

LD = $(CXX)
LIB = -lze_loader

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
