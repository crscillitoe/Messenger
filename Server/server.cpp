#include "server.hpp"


const char* DEFAULT_PORT = "8371";
pthread_mutex_t lock;
json jtoSend;
vector<string> connectedUsers;
clients connections;

int main(int argc, const char *argv[]) {

    jtoSend = makeJson(" ", " ", &connectedUsers, 1);

	short PORT;
	int socketDescriptor;
	struct sockaddr_in clientAddress;
	unsigned int clientLength;
	int clientSocket;


	validateInput(argc, argv);

	if(argc == 1) {
		argv[1] = DEFAULT_PORT; 
	}

	//Initialize the mutex variable, lock.
	if(pthread_mutex_init(&lock , NULL) != 0) {
		printf("Mutex init failed.\n");
		return -1; //Indicate FAILURE.
	}

	//Convert the given port into a short.
	PORT = htons(atoi(argv[1]));

	if((socketDescriptor = initConnection(&socketDescriptor, PORT)) == -1) {
		return -1;
	}


	// Create thread to update clients
	pthread_t serverThread;
	if(pthread_create(&serverThread , NULL , updateClients , NULL) != 0) {
		printf("Pthread creation error!\n");
		return -1; //Indicate FAILURE.
	}

	//Create threads to read from clients
	while(1) {
		pthread_t temp;
		clientLength = sizeof(clientAddress);
		if((clientSocket = accept(socketDescriptor , (struct sockaddr *) &clientAddress, &clientLength)) < 0) {
			printf("Accept() failed");
			return -1; //Indicate FAILURE.
		}
		if(pthread_create(&temp , NULL , clientThread , (void*)(&clientSocket)) != 0) {
			printf("Pthread creation error!\n");
			return -1; //Indicate FAILURE.
		}
	}

	if(pthread_join(serverThread , NULL)) {
		printf("Error joining threads.\n");
		return -1; //Indicate FAILURE.
	}

}

void* updateClients(void*) {
	printf("updateClients has started!\n");
	string currentUser;
	int currentSeqNum = 0;

	while(1) {
		usleep(5000);
		pthread_mutex_lock(&lock);

		if(currentSeqNum != jtoSend["seqnum"] || (jtoSend["username"] != currentUser)){
			//If the messages are different
			int i;
			for(i = 0 ; i < connections.totalConnectedClients ; i++) {
				sendJson(jtoSend, connections.socketDescriptors[i]);
			}
			currentUser = jtoSend["username"];
			currentSeqNum = jtoSend["seqnum"];
		}
		pthread_mutex_unlock(&lock);
	}
}



void* clientThread(void* val) {
	int MAX_MESSAGE_SIZE = 2048;
	char bufferRead[MAX_MESSAGE_SIZE];

	json wrap;
	int seqnum;
	string username;
	string buffer;

	int SOCKET_ID = *((int*)val);
	printf("Child with Socket ID %d has connected!\n" , SOCKET_ID);
	int running = 1;

	pthread_mutex_lock(&lock);
	connections.socketDescriptors[connections.totalConnectedClients] = SOCKET_ID;
	connections.totalConnectedClients++;
	pthread_mutex_unlock(&lock);

	while(running) {
		bzero(bufferRead, MAX_MESSAGE_SIZE);

		read(SOCKET_ID , bufferRead, MAX_MESSAGE_SIZE);
		printf("Buffer Read : %s\n", bufferRead);
		auto wrap = json::parse(bufferRead);

		if(wrap["seqnum"] != seqnum){
			seqnum = wrap["seqnum"];
		}
		username = wrap["username"];
		buffer = wrap["message"];

		if(buffer == "EXIT\n"){
			running = 0;
			//pop username
			connectedUsers.erase(std::remove(connectedUsers.begin(), connectedUsers.end(), username), connectedUsers.end());	

			pthread_mutex_lock(&lock);	
			jtoSend = makeJson("SYSTEM", username + " HAS DISCONNECTED\n", &connectedUsers, seqnum);
			pthread_mutex_unlock(&lock);
		} else {

			pushUnique(&connectedUsers, username);

			pthread_mutex_lock(&lock);	
			jtoSend = makeJson(username, buffer, &connectedUsers, seqnum);
			pthread_mutex_unlock(&lock);
		}
	}

	pthread_mutex_lock(&lock);
	connections.totalConnectedClients--;
	int i;
	int location = MAX_CONNECTIONS;
	for(i = 0 ; i < MAX_CONNECTIONS - 1 ; i++) {
		if(connections.socketDescriptors[i] == SOCKET_ID) {
			location = i;
			break;
		}
	}
	for(i = location ; i < MAX_CONNECTIONS ; i++) {
		connections.socketDescriptors[i] = connections.socketDescriptors[i + 1];
	}
	pthread_mutex_unlock(&lock);

	printf("Child with Socket ID %d has disconnected!\n" , SOCKET_ID);
	shutdown(SOCKET_ID , SHUT_RDWR);
	close(SOCKET_ID);

	return NULL;
}


