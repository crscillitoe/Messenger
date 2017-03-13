#Test

CC=g++
CFLAGS=-g -c -Wall -Wextra -std=c++11
LDFLAGS= -pthread -std=c++11
SOURCES=server.cpp server.hpp primitives.cpp
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=server

all: $(SOURCES) $(EXECUTABLE) cleanup
    
$(EXECUTABLE): $(OBJECTS) 
		$(CC) $(LDFLAGS) $(OBJECTS) -o $@ -lncurses

.cpp.o:
		$(CC) $(CFLAGS) $< -o $@

cleanup:
		rm *.o 

clean:
		rm *.o
