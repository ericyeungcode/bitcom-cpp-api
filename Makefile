#!/usr/bin/make -f

#################################
# require boost lib
# linux(ubuntu): sudo apt install libboost-all-dev
#################################


# use websocketpp in vcpkg (websocketpp is header-only library)

UNAME := $(shell uname)

CC=g++
SOURCE_DIR=src
SOURCES = $(wildcard *.cpp) $(wildcard */*.cpp)

ifeq ($(UNAME), Linux)
	WSCPP = $(HOME)/vcpkg/packages/websocketpp_x64-linux/include
	CPPFLAGS = -std=c++17 -Iinclude -Ithirdparty -I/usr/local/opt/openssl/include -I$(WSCPP) -Wno-deprecated-declarations
	LDLIBS = -lssl -lcrypto -L/usr/lib
endif

ifeq ($(UNAME), Darwin)
	WSCPP = $(HOME)/vcpkg/packages/websocketpp_x64-osx/include
	CPPFLAGS = -std=c++17 -Iinclude -Ithirdparty -I$(WSCPP) -Wno-deprecated-declarations
	LDLIBS = -lssl -lcrypto -L/usr/local/lib -L/usr/local/opt/openssl/lib
endif



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