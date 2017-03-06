#include "client.hpp"

pthread_mutex_t lock;

int serverSocket;

int main(int argc, char* argv[])
{


	unsigned short serverPort;
	char* str_port; 
	char* username; 
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

	}
	else
	{
		serverPort = 8371;
	}

	username = argv[2];
	url = argv[1];

	serverSocket = initConnection(serverPort, url, serverSocket);


	initscr();
	raw();

	pthread_t thread;
	pthread_create(&thread , NULL , readThread , (void*) &serverSocket);

	inputLoop(serverSocket, username);

	endwin();

	//  free(s);
}

void cleanUpAndExit(int){ //int serverSocket){

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

		drawLines();
		mvprintw(4 , COLS - 18 ,  connectedUser.c_str());


		move(LINES - 4 , 2);

		refresh();
	}
}
