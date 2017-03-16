#include "client.hpp"

//Initialize the connectsion with the remote server
int initConnection(unsigned short serverPort, char* url, int serverSocket) {

	struct sockaddr_in serverAddr;
	struct hostent *hp;
	// Socket
	if((serverSocket = socket(AF_INET, SOCK_STREAM, 0)) < 0){
		fprintf(stderr, "Socket error: %s\n", strerror(errno));
		exit(1);
	}

	// Initialize struct
	memset(&serverAddr, 0, sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	hp = gethostbyname(url);

	if(hp == NULL){
		fprintf(stderr, "Error: specified URL does not exist\n");
		exit(1);
	}

	bcopy ( (char*)hp->h_addr,
			(char *) &serverAddr.sin_addr.s_addr,
			hp->h_length);
	serverAddr.sin_port = htons(serverPort);

	// Connect to server socket
	if(connect(serverSocket, (struct sockaddr *) &serverAddr, sizeof(serverAddr)) < 0){
		fprintf(stderr, "Connect error: %s\n", strerror(errno));
		exit(1);
	}

	return serverSocket;
}

//add a line to the display buffer (lines) without going over maxmum lines
int writeLine(string buffer, int linesUsed, string lines[]){
	
	const int linesToRemember = (LINES - 7);
	int i;
	for(i = linesUsed ; i > 0 ; i--) {
		lines[i] = lines[i - 1];
	}

	lines[0] = buffer.c_str();
	if(linesUsed < linesToRemember) {
		linesUsed++;
	}

	return linesUsed;
}

void printn(const char* message) {
	printf("%s\n" , message);
}

// Draw the ncurses message window
void drawScreen(){
	int i;

	mvprintw(LINES - 5 , 0 , " ");
	mvprintw(LINES - 3 , 0 , " ");
	for(i = 1 ; i < COLS ; i++) {
		mvprintw(1 , i , "-");
		mvprintw(LINES - 5 , i , "-");
		mvprintw(LINES - 3 , i , "-");
		if(i != 1 && i != COLS-20 && i != COLS -1) {
			mvprintw(LINES - 2 , i , " ");
			mvprintw(LINES - 1 , i , " ");
		}

	}

	drawLines();
	mvprintw(LINES - 4 , 1 , ">");
}

//Draw the ncurses columns
void drawLines(){

	int i;
	for(i = 2 ; i < LINES ; i++) {
		if(i != LINES-5 && i != LINES-4 && i != LINES-3) {
			mvprintw(i , 0 , " ");
			mvprintw(i , 1 , "|");
			mvprintw(i , COLS - 1, "|");
			mvprintw(i , COLS - 20 , "|");
		}
	}

	mvprintw(2 , COLS - 18 , "CONNECTED USERS");
	mvprintw(3 , COLS - 20 , "--------------------");
}

//Ask the user for input to send to the server
void inputLoop(int sSocket, char* username) {
	int ex = 0;
	char userinput[2030];
	memset(userinput, '\0', sizeof(userinput));
	int seqnum = 1;
	while(!ex){
		drawScreen();
		bzero(userinput , 2030);
		mvgetnstr(LINES-4, 2, userinput , 2030);
		userinput[strlen(userinput)] = '\n';

		int i;
		for(i = 2 ; i < COLS ; i++) {
			mvprintw(LINES - 4 , i , " ");
		}

		if(strcmp(userinput, "EXIT\n") == 0){
			return;
			//cleanUpAndExit(0);
		}

		json wrap = makeJson(username, userinput, seqnum);
		if(sendJson(wrap, sSocket)){
			exit(1);
		}
		seqnum = !seqnum; 
	}
}

//Send the given json to the given connected socket
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

//Compile raw inputs into a json
json makeJson(string user, string message, int seqnum){
	json wrap;
	wrap["username"] = user;
	wrap["message"] = message;
	wrap["seqnum"] = seqnum;
	return wrap;
}

//Clear the "connected users" section of the screen between messages
void clearConnectedUsers(){
	int j;
	for(j = 4; j < LINES-6; j++) {
		mvprintw(j, COLS - 18 , "                 ");
	}
}

//Print all the currently connected users
void printConnectedUsers(vector<string> *users) {
	int j = 0;
	std::vector<string>::iterator itr;
	for ( itr = (*users).begin(); itr != (*users).end(); ++itr ) {
		mvprintw(j+4 , COLS - 18 ,  (*users)[j].c_str());
		j++;
	}
}

//Let the server know that you have joined the room
void sendInitialMessage(char* myUsername, int serverSocket){
	string usr;
	usr.assign(myUsername, strlen(myUsername));
	string initialMessage = usr + " Has Joined the room\n";
	json init = makeJson(usr, initialMessage, 0);
	if(sendJson(init, serverSocket)){
		exit(1);
	}
}
