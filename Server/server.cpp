#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/tcp.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <unistd.h>
#include <sys/time.h>
#include <fcntl.h>
#include <netdb.h>
#include <pthread.h>
#include <string>
#include "json.hpp"
using namespace std;
using json = nlohmann::json;

const int MAX_CONNECTIONS = 50;
pthread_mutex_t lock;
int totalConnectedClients = 0;
int socketDescriptors[MAX_CONNECTIONS];

json jtoSend;
void* updateClients(void* val);
void* clientThread(void* val);
int main(int argc, const char *argv[]) {

        jtoSend["username"] = " ";
        jtoSend["seqnum"] = 1;
        jtoSend["message"] = " ";
        short PORT;
        int socketDescriptor;

        struct sockaddr_in clientAddress;
        unsigned int clientLength;
        int clientSocket;

        struct sockaddr_in serverAddress;

        if(argc != 2 && argc != 1) {
                printf("INVALID SYNTAX\n");
                printf("Valid syntax : %s PORT\n" , argv[0]);
                printf("OR : %s\n" , argv[0]);
                return -1; //Indicate FAILURE.
        }

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

        int MAX_MESSAGE_SIZE = 2048;
        char buffer[MAX_MESSAGE_SIZE];
        pid_t PID;

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

void* clientThread(void* val) {
        int MAX_MESSAGE_SIZE = 2048;
        char bufferRead[MAX_MESSAGE_SIZE];

        json wrap;
        int seqnum;
        string username;
        string buffer;

        int SOCKET_ID = *((int*)val);
        printf("Child with Socket ID %d has connected!\n" , SOCKET_ID);
        //        bzero(buffer , MAX_MESSAGE_SIZE);
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
                        buffer = wrap["message"];

                        const char* pr_temp = buffer.c_str();
                        printf("BUFFER : %s\n" , pr_temp);
                        string bufferCPPString = buffer;
                        pthread_mutex_lock(&lock);
                        jtoSend["username"] = username;
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
                if(socketDescriptors[i] = SOCKET_ID) {
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
