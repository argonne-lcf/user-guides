#
# Definition of MACROS

CXX = mpicxx
CXXFLAGS = -std=c++20
CXXFLAGS += --intel -fsycl #-fsycl-unnamed-lambda
CXXFLAGS += -fsycl-targets=spir64_gen

CPP = cpp -P -traditional
CPPFLAGS =

LD = $(CXX) -Xsycl-target-backend "-device 12.60.7"
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
