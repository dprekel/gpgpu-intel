TARGET = gpgpu-intel
CC = g++
CFLAGS = -g -Wall
LDFLAGS = -lOpenCL

.PHONY: default all clean

default: $(TARGET)
all: default

OBJECTS = $(patsubst %.cpp, %.o, $(wildcard *.cpp))
HEADERS = $(wildcard *.h)

%.o: %.cpp $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@ $(LDFLAGS)

.PRECIOUS: $(TARGET) $(OBJECTS)

$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) -o $@ $(LDFLAGS)

clean:
	-rm -f *.o
	-rm -f $(TARGET)
