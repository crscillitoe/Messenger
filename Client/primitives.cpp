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
			
		//  exit(0);
		}

		json wrap = makeJson(username, userinput, seqnum);

		if(sendJson(wrap, sSocket)){
			exit(1);
		}

		seqnum = !seqnum; 
	}


}

int sendJson(json j, int socket){	
		string toSendcpp = j.dump();
		const char* toSend = toSendcpp.c_str();
		if(write(socket, toSend, strlen(toSend)) < 0)
		{
			fprintf(stderr, "Write returned an error: %s\n", strerror(errno));
			return -1;
		}
		return 0;
}

json makeJson(string user, string message, int seqnum){
	json wrap;
	wrap["username"] = user;
	wrap["message"] = message;
	wrap["seqnum"] = seqnum;
	return wrap;
}
