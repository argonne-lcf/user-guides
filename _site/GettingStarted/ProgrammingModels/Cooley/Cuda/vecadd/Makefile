#
# Definition of MACROS

CUDA = $(CUDA_PATH)

CXX = mpicxx
CXXFLAGS = -g -O3 -std=c++0x -I$(CUDA)/include
CXXFLAGS += -D_SINGLE_PRECISION

CUDA_CXX = nvcc
CUDA_CXXFLAGS = 
CUDA_CXXFLAGS += -D_SINGLE_PRECISION

CPP = cpp -P -traditional
CPPFLAGS =

LD = $(CXX)
LIB = -L$(CUDA)/lib64 -lcuda -lcudart

BINROOT=./
EX=vecadd
SHELL=/bin/sh

# -- subset of src files with cuda kernels
CUDA_SRC = main.cpp
CUDA_OBJ = $(CUDA_SRC:.cpp=.o)

SRC = $(filter-out $(CUDA_SRC), $(wildcard *.cpp))
INC = $(wildcard *.h)
OBJ = $(SRC:.cpp=.o)

#
# -- target : 	Dependencies
# --		Rule to create target

$(EX): 	$(OBJ) $(CUDA_OBJ)
	$(LD) -o $@ $(CXXFLAGS) $(OBJ) $(CUDA_OBJ) $(LIB)

####################################################################

$(OBJ): %.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $<

$(CUDA_OBJ): %.o: %.cpp
	$(CUDA_CXX) -x cu $(CUDA_CXXFLAGS) -c $< -o $@

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
$(CUDA_OBJ) : $(INC)
