CC = g++
CFLAGS = -g -Wall -fPIC -mavx2
LDFLAGS = -shared
PREFIX = /usr/local

.PHONY: default all clean install

TARGET = libigpgpu.so
SOURCES = $(wildcard *.cpp) $(wildcard */*.cpp)
OBJECTS = $(SOURCES:.cpp=.o) $(*/SOURCES:.cpp=.o)

default: $(TARGET)

%.o: %.cpp
	$(CC) $(CFLAGS) -c $< -o $@

$(TARGET): $(OBJECTS)
	$(CC) $(LDFLAGS) -o $@ $^

install:
	install -m 557 $(TARGET) $(PREFIX)/lib/
	install -m 644 gpgpu.h $(PREFIX)/include/

clean:
	-rm -f *.o
	-rm -f $(TARGET)
	-rm -f $(PREFIX)/lib/$(TARGET)
	-rm -f $(PREFIX)/include/gpgpu.h

# $@ denotes the name of the target
# $< denotes the first prerequisit



