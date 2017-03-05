#ifndef SERVER
#define SERVER

#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/tcp.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <unistd.h>
#include <sys/time.h>
#include <fcntl.h>
#include <netdb.h>
#include <pthread.h>
#include <string>

using namespace std;
// Globals
const int MAXUSERNAMESIZE = 16;
const int MAX_CONNECTIONS = 50;

//Structs
typedef struct ll {
	string user;
	struct ll* next;
} stringll;
	


// Function Prototypes

int push(stringll** head, string newUser);
void  allocateArray(char** arr, int num_elements, int sizeof_elements);
void* updateClients(void* val);
void* clientThread(void* val);
void initJson();
void validateInput(int argc, const char* argv[]);
int initConnection(int* sd, short PORT);
#endif
