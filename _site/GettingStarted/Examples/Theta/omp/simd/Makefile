CC = CC

CFLAGS = -qopenmp -dynamic
VEC = -DOMP_SIMD
NOVEC = -no-vec
MEMALIGN = -DMEM_ALIGN

TARGET = simd.novec simd.vec simd.vec.mem.align 

all: $(TARGET)

simd.novec : main.cpp
	$(CC) $(CFLAGS) $(NOVEC) $< -o $@
simd.vec : main.cpp
	$(CC) $(CFLAGS) $(VEC) $< -o $@
simd.vec.mem.align : main.cpp
	$(CC) $(CFLAGS) $(VEC) $(MEMALIGN) $< -o $@

clean :
	rm -rf *.o $(TARGET)
