
CC = g++
CFLAGS = -g -Wall -fPIC
LDFLAGS = -shared

.PHONY: default all clean

TARGET = gpgpu-intel.so
SOURCES = $(wildcard *.cpp)
HEADERS = $(wildcard *.h)
OBJECTS = $(SOURCES:.cpp=.o)

default: $(TARGET)

$(OBJECTS): $(SOURCES) $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@ 

$(TARGET): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

clean:
	-rm -f *.o
	-rm -f $(TARGET)

# $@ denotes the name of the target
# $< denotes the first prerequisit



