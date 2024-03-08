CC = g++
CFLAGS = -g -Wall -fPIC
LDFLAGS = -shared
PREFIX = /usr/local
INC = -I/usr/local/include/igc -I/usr/local/include/igc/cif

.PHONY: default all clean install

TARGET = libigpgpu.so
SOURCES = $(wildcard *.cpp)
OBJECTS = $(SOURCES:.cpp=.o)

default: $(TARGET)

%.o: %.cpp
	$(CC) $(CFLAGS) $(INC) -c $< -o $@

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



