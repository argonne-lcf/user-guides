# Use LLVM compiler w/ ALCF provided OpenMPI
#
# module load llvm
# module load cudatoolkit-standalone
# module load mpiwrappers/cray-mpich-llvm
#
# Definition of MACROS

CXX = mpicxx
CXXFLAGS = -g -fsycl -fsycl-targets=nvptx64-nvidia-cuda -Xsycl-target-backend --cuda-gpu-arch=sm_80
CXXFLAGS += -std=c++20
#CXXFLAGS += -D_SINGLE_PRECISION

CPP = cpp -P -traditional
CPPFLAGS =

LD = $(CXX)
LIB = 
#LIB += -L/opt/cray/pe/mpich/8.1.16/gtl/lib -lmpi_gtl_cuda
LIB += $(PE_MPICH_GTL_DIR_nvidia80) $(PE_MPICH_GTL_LIBS_nvidia80)

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
