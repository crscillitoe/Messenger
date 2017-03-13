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
#include <algorithm>
#include <vector>
#include "json.hpp"
using namespace std;
using json = nlohmann::json;
// Globals
const int MAXUSERNAMESIZE = 16;
const int MAX_CONNECTIONS = 50;

//Structs

typedef struct _connected {
	int totalConnectedClients;
	int socketDescriptors[MAX_CONNECTIONS];
} clients;


// Function Prototypes
int pushUnique(vector<string> *vec, string toAdd);
void  allocateArray(char** arr, int num_elements, int sizeof_elements);
void* updateClients(void* val);
void* clientThread(void* val);
void validateInput(int argc, const char* argv[]);
int initConnection(int* sd, short PORT);
int sendJson(json j, int socket);
json makeJson(string user, string message, vector<string> *vec, int seqnum);
void addConnection(clients *c, int sid);
void removeConnection(clients *c, int sid);
#endif
