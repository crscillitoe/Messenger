#ifndef CLIENT
#define CLIENT

//Includes
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <errno.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string>
#include <ncurses.h>
#include <signal.h>
#include <vector>

#include "json.hpp"
using namespace std;
using json = nlohmann::json;

void printConnectedUsers(vector<string> *users);
int sendJson(json j, int socket);
json makeJson(string user, string message, int seqnum);
void* readThread(void* val);
void cleanUpAndExit(int);
void printn(const char* message);
int initConnection(unsigned short serverPort, char* url, int serverSocket);
void drawScreen();
void drawLines();

void sendInitialMessage(char* myUsername, int serverSocket);
void clearConnectedUsers();
int writeLine(string buffer, int linesUsed, string lines[]);
void inputLoop(int sSocket, char* username);

#endif
