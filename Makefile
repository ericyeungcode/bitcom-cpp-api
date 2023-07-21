#!/usr/bin/make -f

# use websocketpp in vcpkg (websocketpp is header-only library)
WSCPP = /Users/ericyeung/vcpkg/packages/websocketpp_x64-osx/include
DISABLE_SSL1_WARN = -Wno-deprecated-declarations

CC=g++
SOURCE_DIR=src
SOURCES = $(wildcard *.cpp) $(wildcard */*.cpp)

CPPFLAGS = -std=c++17 -Iinclude -I/usr/local/opt/openssl/include -I$(WSCPP) $(DISABLE_SSL1_WARN)
LDLIBS = -lssl -lcrypto -L/usr/local/lib -L/usr/local/opt/openssl/lib

EXECUTABLE = main
# pattern substitute
OBJECTS = $(SOURCES:.cpp=.o)

all: $(OBJECTS) $(EXECUTABLE)

$(EXECUTABLE) : $(OBJECTS)
		$(CC) $(OBJECTS) $(LDLIBS) -v -o $@ 

# -c -o: compile
.cpp.o: *.h
	$(CC) $(CPPFLAGS) $< -c -o $@

clean:
	@echo Cleaning
	-rm -f $(OBJECTS) $(EXECUTABLE)
	@echo Cleaning done
	
.PHONY: all clean