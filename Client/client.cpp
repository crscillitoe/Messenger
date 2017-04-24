#include "client.hpp"

pthread_mutex_t lock;

int serverSocket;

char* myUsername; 
int main(int argc, char* argv[]){

	char* url = "ec2-35-164-26-58.us-west-2.compute.amazonaws.com";
	unsigned short serverPort;
	char* str_port; 

	signal(SIGINT, cleanUpAndExit);

	//validate inputs
	if(argc < 2 || argc > 4){
		fprintf(stderr, "Usage %s USERNAME [-p PORT]\n", argv[0]);
		exit(1);
	}

	//Set custom port
	if(argc == 4){
		str_port = argv[4];
		// Convert Port from string to short
		char *ptr;
		serverPort = strtol(str_port, &ptr, 10);
	} else {
		serverPort = 8371;
	}

	myUsername = argv[1];
	//url = IP;

	//Make the connection with the server
	serverSocket = initConnection(serverPort, url, serverSocket);


	initscr();
	raw();

	pthread_t thread;
	pthread_create(&thread , NULL , readThread , (void*) &serverSocket);

	//drawScreen();
	sendInitialMessage(myUsername, serverSocket);

	inputLoop(serverSocket, myUsername);

	//if(pthread_join(thread , NULL)) {
	//	printf("Error joining threads.\n");
	//}

	cleanUpAndExit(0);
}


//Exit gracefully
void cleanUpAndExit(int){ 

	json wrap = makeJson(myUsername, "EXIT\n", 2);
	if(sendJson(wrap, serverSocket)){
		fprintf(stderr, "Could not send EXIT message\n");
	}

	endwin();
	exit(0);

//	shutdown(serverSocket, 0);
//	close(serverSocket);
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

		//Parse the json received
		auto recv = json::parse(buffer);
		string username = recv["username"];
		string message = recv["message"];
		std::vector<string> connectedUser = recv["users"];

		//Create message to print
		string buff = username + ": " + message;

		//Add new message to th lines buffer
		int bufferSize = buff.length();
		if(bufferSize <= lineLength) { //one line message
			linesUsed = writeLine(buff, linesUsed, lines);
		} else if(bufferSize > lineLength) { //two line message
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

