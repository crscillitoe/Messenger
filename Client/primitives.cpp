#include "client.hpp"

int initConnection(unsigned short serverPort, char* url, int serverSocket) {

	struct sockaddr_in serverAddr;
	struct hostent *hp;
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

	return serverSocket;

}

void printn(const char* message) {
	printf("%s\n" , message);
}

void drawScreen(){

		int i;
		for(i = 1 ; i < COLS ; i++) {
			mvprintw(1 , i , "-");
			mvprintw(LINES - 5 , i , "-");
			mvprintw(LINES - 3 , i , "-");
		}

		drawLines();

		mvprintw(LINES - 4 , 1 , ">");
}

void drawLines(){

	int i;
	for(i = 2 ; i < LINES ; i++) {
		if(i != LINES-5 && i != LINES-4 && i != LINES-3) {
			mvprintw(i , 1 , "|");
			mvprintw(i , COLS - 1, "|");
			mvprintw(i , COLS - 20 , "|");
		}
	}

	mvprintw(2 , COLS - 18 , "CONNECTED USERS");
	mvprintw(3 , COLS - 20 , "--------------------");
}

void inputLoop(int sSocket, char* username) {


	int ex = 0;
	char userinput[2030];
	memset(userinput, '\0', sizeof(userinput));
	char message_to_send[2048];

	memset(message_to_send, '\0', sizeof(message_to_send));
	int seqnum = 0;
	while(!ex)
	{

		drawScreen();

		bzero(userinput , 2030);
		bzero(message_to_send, 2048);
		getnstr(userinput , 2030);

		userinput[strlen(userinput)] = '\n';

		int i;
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
		if(write(sSocket, toSend, strlen(toSend)) < 0)
		{
			fprintf(stderr, "Write returned an error: %s\n", strerror(errno));
			exit(1);
		}

		seqnum = !seqnum; 
	}


}

