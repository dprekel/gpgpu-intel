CC = g++
CFLAGS = -Wall -fPIC -fvisibility=hidden -mavx2
LDFLAGS = -shared
PREFIX = /usr/local
ifeq ($(DEBUG), 1)
	CFLAGS += -g -DDEBUG
endif
ifeq ($(WARNINGS), 1)
	CFLAGS += -pedantic -Wextra -Wcast-align -Wcast-qual -Wctor-dtor-privacy -Wdisabled-optimization -Wformat=2 -Winit-self -Wlogical-op -Wmissing-declarations -Wmissing-include-dirs -Wnoexcept -Wold-style-cast -Woverloaded-virtual -Wredundant-decls -Wshadow -Wsign-conversion -Wsign-promo -Wstrict-null-sentinel -Wstrict-overflow=5 -Wswitch-default -Wundef -Wno-unused
endif

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



