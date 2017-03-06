#Test

CC=g++
CFLAGS=-c -Wall -std=c++11 
LDFLAGS= -pthread -std=c++11 
SOURCES=client.cpp json.hpp client.hpp primitives.cpp
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=client

all: $(SOURCES) $(EXECUTABLE) cleanup
    
$(EXECUTABLE): $(OBJECTS) 
		$(CC) $(LDFLAGS) $(OBJECTS) -o $@ -lncurses

.cpp.o:
		$(CC) $(CFLAGS) $< -o $@

cleanup:
		rm *.o 

clean:
		rm *.o
