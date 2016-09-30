#include <stdio.h>


pthread_mutex_t lock;

string toSend;

int main(int argc, const char *argv[]) {

	short PORT;
	int socketDescriptor;

	struct sockaddr_in clientAddress;
	unsigned int cliengLength;

	struct sockaddr_in serverAddress;

	if(argc != 2) {
		printf("INVALID SYNTAX\n");
		printf("Valid syntax : %s PORT\n" , argv[0]);
		return -1; //Indicate FAILURE.
	}

	//Initialize the mutex variable, lock.
	if(pthread_mutex_init(&lock , NULL) != 0) {
		printf("Mutex init failed.\n");
		return -1; //Indicate FAILURE.
	}

	//Convert the given port into a short.
	PORT = htons(atoi(argv[1]));

	memset(&serverAddress , 0 , sizeof(serverAddress));
	serverAddress.sin_family = AF_INET;
	serverAddress.sin_addr.s_addr = htonl(INADDR_ANY);
	serverAddress.sin_port = PORT;

	if((socketDescriptor = socket(PF_INET , SOCK_STREAM , IPPROTO_TCP)) < 0) {
		printf("Error, failed to open socket.\n");
		return -1; //Indicate FAILURE.
	}

	if(bind(socketDescriptor , (struct sockaddr *)&serverAddress, sizeof(serverAddress)) < 0) {
		printf("Error, process crashed during bind() call.\n");
		return -1; //Indicate FAILURE.
	}

	if(listen(socketDescriptor , 1) < 0) {
		printf("Error, process crashed during listen() call.\n");
		return -1; //Indicate FAILURE.
	}

	int MAX_MESSAGE_SIZE = 2048;
	char buffer[MAX_MESSAGE_SIZE];
	pid_t PID;

	while(1) {
		clientLength = sizeof(clientAddress);
		if((clientSocket = accept(socketDescriptor , (struct sockaddr *) &clientAddress, &clientLength)) < 0) {
			printf("Accept() failed");
			return -1; //Indicate FAILURE.
		}
		if((PID = fork()) < 0) {
			printf("Fork error\n");
			return -1; //Indicate FAILURE.
		} else if(PID == 0) {
			//CHILD PROCESS
			bzero(buffer , MAX_MESSAGE_SIZE);
			read(clientSocket , buffer , MAX_MESSAGE_SIZE);
			string bufferCPPString = buffer;
			pthread_mutex_lock(&lock);
			toSend = buffer;
			pthread_mutex_unlock(&lock);
		}
	}
}
