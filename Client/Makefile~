CC=gcc
CFLAGS=-c -Wall
LDFLAGS=
SOURCES=client.c
OBJECTS=$(SOURCES:.c=.o)
EXECUTABLE=client

all: $(SOURCES) $(EXECUTABLE) cleanup
    
$(EXECUTABLE): $(OBJECTS) 
		$(CC) $(LDFLAGS) $(OBJECTS) -o $@

.c.o:
		$(CC) $(CFLAGS) $< -o $@

cleanup:
		rm *.o 

clean:
		rm *.o
