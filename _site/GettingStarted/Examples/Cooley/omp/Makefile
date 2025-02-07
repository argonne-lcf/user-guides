CC = mpicxx

CFLAGS = -fopenmp

OBJS = main.o

TARGET = hello_affinity

all: $(TARGET)

main.o : main.cpp
	$(CC) $(CFLAGS) -c main.cpp -o $@

$(TARGET) : $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $@

clean : 
	rm -rf hello_affinity *.o
