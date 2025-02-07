CC = CC

CFLAGS = -g

OBJS = main.o

TARGET = main

all: $(TARGET)

main.o : main.cpp
	$(CC) $(CFLAGS) -c $< -o $@

$(TARGET) : $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $@

clean :
	rm -rf *.o main
