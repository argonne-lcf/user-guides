CC = CC

CFLAGS = -qopenmp -dynamic

TARGET = pi approximate_bandwidth

all: $(TARGET)

pi : pi.cpp
	$(CC) $(CFLAGS) $< -o $@
approximate_bandwidth : approximate_bandwidth.cpp
	$(CC) -dynamic -mcmodel=large $(CFLAGS) $< -o $@

clean :
	rm -rf *.o $(TARGET)
