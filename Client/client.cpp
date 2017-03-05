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

void* readThread(void* val);
void cleanUpAndExit(int);

pthread_mutex_t lock;

int serverSocket;

int main(int argc, char* argv[])
{

	struct sockaddr_in serverAddr;
	struct hostent *hp;

	unsigned short serverPort;
	char* str_port; //[6];
	char* username; //[13];
	char* url; //[17];


	signal(SIGINT, cleanUpAndExit);


	if(argc < 3 || argc > 5)
	{
		fprintf(stderr, "Usage %s serverIP USERNAME [-p PORT]\n", argv[0]);
		exit(1);
	}

	if(argc == 5)
	{
		str_port = argv[4];
		// Convert Port from string to short
		char *ptr;
		serverPort = strtol(str_port, &ptr, 10);

	}
	else
	{
		serverPort = 8371;
	}

	username = argv[2];
	url = argv[1];



	// Socket
	if((serverSocket = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		fprintf(stderr, "Socket error: %s\n", strerror(errno));
		exit(1);

	}

	// Initialize struct
	memset(&serverAddr, 0, sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	hp = gethostbyname(url);

	if(hp == NULL)
	{
		fprintf(stderr, "Error: specified URL does not exist\n");
		exit(1);

	}

	bcopy ( (char*)hp->h_addr,
			(char *) &serverAddr.sin_addr.s_addr,
			hp->h_length);
	serverAddr.sin_port = htons(serverPort);

	// Connect to server socket
	if(connect(serverSocket, (struct sockaddr *) &serverAddr, sizeof(serverAddr)) < 0)
	{

		fprintf(stderr, "Connect error: %s\n", strerror(errno));
		exit(1);
	}

	int ex = 0;
	char userinput[2030];
	memset(userinput, '\0', sizeof(userinput));


	char message_to_send[2048];

	memset(message_to_send, '\0', sizeof(message_to_send));

	initscr();
	raw();

	pthread_t thread;
	pthread_create(&thread , NULL , readThread , (void*) &serverSocket);
	int seqnum = 0;
	while(!ex)
	{

		int i;
		for(i = 1 ; i < COLS ; i++) {
			mvprintw(1 , i , "-");
			mvprintw(LINES - 5 , i , "-");
			mvprintw(LINES - 3 , i , "-");
		}

		for(i = 2 ; i < LINES ; i++) {
			if(i != LINES-5 && i != LINES-4 && i != LINES-3) {
				mvprintw(i , 1 , "|");
				mvprintw(i , COLS - 1, "|");
				mvprintw(i , COLS - 20 , "|");
			}
		}

		mvprintw(2 , COLS - 18 , "CONNECTED USERS");
		mvprintw(3 , COLS - 20 , "--------------------");

		mvprintw(LINES - 4 , 1 , ">");

		bzero(userinput , 2030);
		bzero(message_to_send, 2048);
		getnstr(userinput , 2030);

		userinput[strlen(userinput)] = '\n';

		for(i = 2 ; i < COLS ; i++) {
			mvprintw(LINES - 4 , i , " ");
		}


		if(strcmp(userinput, "EXIT\n") == 0)
		{
			cleanUpAndExit(0);
		}

		json wrap;
		wrap["username"] = username;
		wrap["message"] = userinput;
		wrap["seqnum"] = seqnum;

		string toSendcpp = wrap.dump();
		const char* toSend = toSendcpp.c_str();

		// printf("TO SEND: %s\n", toSend);
		message_to_send[0] = '\0';   // ensures the memory is an empty string
		strcat(message_to_send, username);
		strcat(message_to_send, ": ");
		strcat(message_to_send, userinput);
		// printf("%s", message_to_send);
		// Write request to server socket
		if(write(serverSocket, toSend, strlen(toSend)) < 0)
		{
			fprintf(stderr, "Write returned an error: %s\n", strerror(errno));
			exit(1);
		}

		seqnum = !seqnum; 
	}

	endwin();

	//  free(s);
}

void printn(const char* message) {
	printf("%s\n" , message);
}

void cleanUpAndExit(int){

	(write(serverSocket, "EXIT\n", strlen("EXIT\n")) < 0);
	fflush(stdout);
	exit(0);

}

void* readThread(void* val) {

	int socketID = *((int*)val);
	const int MAX_MESSAGE_LENGTH = 2048;
	char buffer[MAX_MESSAGE_LENGTH];

	const int linesToRemember = (LINES - 7);

	const int MAX_POSSIBLE_LINES_REMEMBER = 100;

	string lines[MAX_POSSIBLE_LINES_REMEMBER];

	const int lineLength = (COLS - 22);
	int linesUsed = 0;

	json recv;
	while(1) {
		bzero(buffer , MAX_MESSAGE_LENGTH);
		read(socketID , buffer , MAX_MESSAGE_LENGTH);

		auto recv = json::parse(buffer);

		string username = recv["username"];
		string message = recv["message"];
		string connectedUser = recv["users"];

		string buff = username + ": " + message;
		//              mvprintw(5, 5, "Buff: %s\n", buff.c_str());


		int bufferSize = buff.length();
		if(bufferSize <= lineLength) {
			int i;
			for(i = linesUsed ; i > 0 ; i--) {
				lines[i] = lines[i - 1];
			}

			lines[0] = buff.c_str();
			if(linesUsed < linesToRemember) {
				linesUsed++;
			}

		} else if(bufferSize > lineLength) {
			int linesToAdd = (bufferSize / lineLength) + 1;
			int i;
			int c;
			for(c = 0 ; c < linesToAdd ; c++) {
				for(i = linesUsed ; i > 0 ; i--) {
					//bzero(lines[i] , MAX_MESSAGE_LENGTH);
					lines[i] = lines[i - 1];
				}
				if(linesUsed < linesToRemember) {
					linesUsed++;
				}
				//bzero(lines[0] , MAX_MESSAGE_LENGTH);
				lines[0] = (buff.substr(c * lineLength , lineLength)).c_str();

			}

		}

		//DRAW TO NCURSES
		int i;
		for(i = 0 ; i < linesUsed ; i++) {
			mvprintw(LINES - (i + 6) , 2 , "%s" , lines[i].c_str());
		}


		for(i = 2 ; i < LINES ; i++) {
			if(i != LINES-5 && i != LINES-4 && i != LINES-3) {
				mvprintw(i , COLS - 1, "|");
				mvprintw(i , COLS - 20 , "|");
			}
		}

		mvprintw(2 , COLS - 18 , "CONNECTED USERS");
		mvprintw(3 , COLS - 20 , "--------------------");
		mvprintw(4 , COLS - 18 ,  connectedUser.c_str());


		move(LINES - 4 , 2);

		refresh();
	}
}
