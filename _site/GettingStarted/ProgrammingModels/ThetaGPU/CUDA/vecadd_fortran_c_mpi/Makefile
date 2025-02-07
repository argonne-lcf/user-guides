#
# Definition of MACROS

CUDA = /usr/local/cuda

F90 = mpif90
F90FLAGS = -g -ffree-form -O3 -I$(CUDA)/include
F90FLAGS += -D_SINGLE_PRECISION

CUDA_CXX = nvcc
CUDA_CXXFLAGS =
CUDA_CXXFLAGS += -D_SINGLE_PRECISION

CPP = cpp -P -traditional
CPPFLAGS =

LD = $(F90)
LIB = -L$(CUDA)/lib64 -lcudart -lstdc++

BINROOT=./
EX=vecadd
SHELL=/bin/sh

# -- subset of src files with cuda kernels
# -- subset of src files with cuda kernels
CUDA_SRC = offload.cpp
CUDA_OBJ = $(CUDA_SRC:.cpp=.o)

SRC = $(wildcard *.F)
MOD = $(wildcard *.mod)
OBJ = $(SRC:.F=.o)

#
# -- target : 	Dependencies
# --		Rule to create target

$(EX): 	$(OBJ) $(CUDA_OBJ)
	$(LD) -o $@ $(F90FLAGS) $(OBJ) $(CUDA_OBJ) $(LIB)

####################################################################

$(OBJ): %.o: %.F
	$(F90) $(F90FLAGS) -c $<

$(CUDA_OBJ): %.o: %.cpp
	$(CUDA_CXX) -x cu $(CUDA_CXXFLAGS) -c $< -o $@

#
# -- Remove *.o and *~ from the directory
clean:
	rm -f *.o *.mod *~
#
# -- Remove *.o, *~, and executable from the directory
realclean:
	rm -f *.o *.mod *~ ./$(EX)

#
# -- Simple dependencies

$(OBJ) : $(MOD)
