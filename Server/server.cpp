#include "server.hpp"
#include "json.hpp"
using json = nlohmann::json;


pthread_mutex_t lock;
int totalConnectedClients = 0;
int socketDescriptors[MAX_CONNECTIONS];
json jtoSend;
vector<string> connectedUsers;
int main(int argc, const char *argv[]) {

	initJson();



	short PORT;
	int socketDescriptor;

	struct sockaddr_in clientAddress;
	unsigned int clientLength;
	int clientSocket;


	validateInput(argc, argv);

	if(argc == 1) {
		argv[1] = "8371";
	}

	//Initialize the mutex variable, lock.
	if(pthread_mutex_init(&lock , NULL) != 0) {
		printf("Mutex init failed.\n");
		return -1; //Indicate FAILURE.
	}

	//Convert the given port into a short.
	PORT = htons(atoi(argv[1]));

	socketDescriptor = initConnection(&socketDescriptor, PORT);

	if(socketDescriptor == -1){
		return -1;
	}
	pthread_t serverThread;
	if(pthread_create(&serverThread , NULL , updateClients , NULL) != 0) {
		printf("Pthread creation error!\n");
		return -1; //Indicate FAILURE.
	}

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

void* updateClients(void* val) {
	printf("updateClients has started!\n");
	string currentUser;
	int currentSeqNum = 0;

	string toSend;
	while(1) {

		usleep(5000);
		pthread_mutex_lock(&lock);

		toSend = jtoSend.dump();


		if(currentSeqNum != jtoSend["seqnum"] || (jtoSend["username"] != currentUser))
		{ //If the messages are different
			int i;
			printf("UPDATECLIENTS TOSEND: %s\n" , toSend.c_str());
			for(i = 0 ; i < totalConnectedClients ; i++) {
				write(socketDescriptors[i] , toSend.c_str(), toSend.length());
			}
			currentUser = jtoSend["username"];
			currentSeqNum = jtoSend["seqnum"];
		}
		pthread_mutex_unlock(&lock);
	}
}



void initJson(){
	jtoSend["username"] = " ";
	jtoSend["seqnum"] = 1;
	jtoSend["message"] = " ";
	jtoSend["users"] = " ";
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
	socketDescriptors[totalConnectedClients] = SOCKET_ID;
	totalConnectedClients++;
	pthread_mutex_unlock(&lock);

	while(running) {
		bzero(bufferRead, MAX_MESSAGE_SIZE);

		read(SOCKET_ID , bufferRead, MAX_MESSAGE_SIZE);
		printf("Buffer Read : %s\n", bufferRead);
		if(!strcmp("EXIT\n", bufferRead))
		{
			running = 0;
		}
		else
		{
			auto wrap = json::parse(bufferRead);

			if(wrap["seqnum"] != seqnum)
			{
				seqnum = wrap["seqnum"];
			}
			username = wrap["username"];
			pushUnique(&connectedUsers, username);
			buffer = wrap["message"];

			const char* pr_temp = buffer.c_str();
			printf("BUFFER : %s\n" , pr_temp);
			string bufferCPPString = buffer;

			pthread_mutex_lock(&lock);
			jtoSend["username"] = username;
			jtoSend["users"] = connectedUsers;
			jtoSend["message"] = bufferCPPString;
			jtoSend["seqnum"] = seqnum;
			pthread_mutex_unlock(&lock);
		}
	}

	pthread_mutex_lock(&lock);
	totalConnectedClients--;
	int i;
	int location = 5000;
	for(i = 0 ; i < MAX_CONNECTIONS - 1 ; i++) {
		if(socketDescriptors[i] == SOCKET_ID) {
			location = i;
			break;
		}
	}
	for(i = location ; i < MAX_CONNECTIONS ; i++) {
		socketDescriptors[i] = socketDescriptors[i + 1];
	}
	pthread_mutex_unlock(&lock);

	printf("Child with Socket ID %d has disconnected!\n" , SOCKET_ID);

	shutdown(SOCKET_ID , SHUT_RDWR);
	close(SOCKET_ID);


}
