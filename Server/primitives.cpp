#include <stdio.h>
#include "server.hpp"

using namespace std;


void addConnection(clients *c, int sid){
	c->socketDescriptors[c->totalConnectedClients] = sid;
	c->totalConnectedClients++;
}


void removeConnection(clients *c, int sid){

	c->totalConnectedClients--;
	int i;
	int location = MAX_CONNECTIONS;
	for(i = 0 ; i < MAX_CONNECTIONS - 1 ; i++) {
		if(c->socketDescriptors[i] == sid) {
			location = i;
			break;
		}
	}
	for(i = location ; i < MAX_CONNECTIONS ; i++) {
		c->socketDescriptors[i] = c->socketDescriptors[i + 1];
	}
}

int pushUnique(vector<string> *vec, string toAdd){
	std::vector<string>::iterator itr;
	for(itr = (*vec).begin(); itr != (*vec).end(); ++itr) {
		if(*itr == toAdd){
			return -1;
		}
	}
	(*vec).push_back(toAdd);
	return 0;
}
int initConnection(int* sd, short PORT){
	int socketDescriptor = *sd;

	struct sockaddr_in serverAddress;

	memset(&serverAddress , 0 , sizeof(serverAddress));
	serverAddress.sin_family = AF_INET;
	serverAddress.sin_addr.s_addr = htonl(INADDR_ANY);
	serverAddress.sin_port = PORT;

	if((socketDescriptor = socket(PF_INET , SOCK_STREAM , IPPROTO_TCP)) < 0) {
		printf("Error, failed to open socket.\n");
		return -1; //Indicate FAILURE.
	}

	if(bind(socketDescriptor , (struct sockaddr *)&serverAddress, sizeof(serverAddress)) < 0) {
		printf("Error, process crashed during bind() call\n");
		return -1; //Indicate FAILURE.
	}

	if(listen(socketDescriptor , 1) < 0) {
		printf("Error, process crashed during listen() call\n");
		return -1; //Indicate FAILURE.
	}

	return socketDescriptor;
}

void  allocateArray(char** arr, int num_elements, int sizeof_elements){
	int i;
	arr = (char**) calloc (num_elements, sizeof(char*));

	for(i = 0; i<MAX_CONNECTIONS; i++){
		arr[i] = (char*) malloc (sizeof_elements * sizeof(char));
	}

}


void validateInput(int argc, const char* argv[]){
	if(argc != 2 && argc != 1) {
		printf("INVALID SYNTAX\n");
		printf("Valid syntax : %s PORT\n" , argv[0]);
		printf("OR : %s\n" , argv[0]);
		exit(1);
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

json makeJson(string user, string message, vector<string> *vec, int seqnum){
	json wrap;
	wrap["username"] = user;
	wrap["message"] = message;
	wrap["users"] = *vec;
	wrap["seqnum"] = seqnum;
	return wrap;
}
