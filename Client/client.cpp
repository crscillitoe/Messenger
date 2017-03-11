#include "client.hpp"

pthread_mutex_t lock;

int serverSocket;

char* myUsername; 
int main(int argc, char* argv[])
{


	unsigned short serverPort;
	char* str_port; 
	char* url; 


	signal(SIGINT, cleanUpAndExit);


	if(argc < 3 || argc > 5){
		fprintf(stderr, "Usage %s serverIP USERNAME [-p PORT]\n", argv[0]);
		exit(1);
	}

	if(argc == 5){
		str_port = argv[4];
		// Convert Port from string to short
		char *ptr;
		serverPort = strtol(str_port, &ptr, 10);
	} else {
		serverPort = 8371;
	}

	myUsername = argv[2];
	url = argv[1];

	serverSocket = initConnection(serverPort, url, serverSocket);


	initscr();
	raw();

	pthread_t thread;
	pthread_create(&thread , NULL , readThread , (void*) &serverSocket);

	drawScreen();
	string usr;
	usr.assign(myUsername, strlen(myUsername));
	string initialMessage = usr + " Has Joined the room\n";
	json init = makeJson(usr, initialMessage, 0);
	if(sendJson(init, serverSocket)){
		exit(1);
	}
	inputLoop(serverSocket, myUsername);

	endwin();

	//  free(s);
}

void cleanUpAndExit(int){ //int serverSocket){

	json wrap = makeJson(myUsername, "EXIT\n", 2);
	if(sendJson(wrap, serverSocket)){
		exit(1);
	}
	exit(0);

}

void* readThread(void* val) {

	//Variables
	int socketID = *((int*)val);
	const int MAX_MESSAGE_LENGTH = 2048;
	char buffer[MAX_MESSAGE_LENGTH];

	const int MAX_POSSIBLE_LINES_REMEMBER = 100;
	const int lineLength = (COLS - 22);
	string lines[MAX_POSSIBLE_LINES_REMEMBER];
	int linesUsed = 0;
	json recv;

	while(1) {
		bzero(buffer , MAX_MESSAGE_LENGTH);
		read(socketID , buffer , MAX_MESSAGE_LENGTH);

		auto recv = json::parse(buffer);
		string username = recv["username"];
		string message = recv["message"];
		std::vector<string> connectedUser = recv["users"];

		string buff = username + ": " + message;

		int bufferSize = buff.length();
		if(bufferSize <= lineLength) {
			linesUsed = writeLine(buff, linesUsed, lines);
		} else if(bufferSize > lineLength) {
			int linesToAdd = (bufferSize / lineLength) + 1;
			int c;
			for(c = 0 ; c < linesToAdd ; c++) {
				linesUsed = writeLine(buff.substr(c * lineLength , lineLength), linesUsed, lines);
			}
		}
		
		//DRAW TO NCURSES
		int i;
		for(i = 0 ; i < linesUsed ; i++) {
			mvprintw(LINES - (i + 6) , 2 , "%s" , lines[i].c_str());
		}
		drawLines();
		clearConnectedUsers();
		printConnectedUsers(&connectedUser);
		move(LINES - 4 , 2);
		refresh();
	}
}

