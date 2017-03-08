#Test

CC=g++
CFLAGS=-g -c -Wall -Wextra
LDFLAGS= -pthread -lncurses
SOURCES=server.cpp server.hpp primitives.cpp
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=server

all: $(SOURCES) $(EXECUTABLE) cleanup
    
$(EXECUTABLE): $(OBJECTS) 
		$(CC) $(LDFLAGS) $(OBJECTS) -o $@

.cpp.o:
		$(CC) $(CFLAGS) $< -o $@

cleanup:
		rm *.o 

clean:
		rm *.o
