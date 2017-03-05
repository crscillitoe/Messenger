#Test

CC=g++
CFLAGS=-c -Wall
LDFLAGS= -pthread -lncurses
SOURCES=client.cpp json.hpp client.hpp primitives.cpp
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=client

all: $(SOURCES) $(EXECUTABLE) cleanup
    
$(EXECUTABLE): $(OBJECTS) 
		$(CC) $(LDFLAGS) $(OBJECTS) -o $@

.cpp.o:
		$(CC) $(CFLAGS) $< -o $@

cleanup:
		rm *.o 

clean:
		rm *.o
