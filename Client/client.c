#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <errno.h>
#include <netinet/in.h>
#include <netdb.h>

void* readThread(void* val);

int main(int argc, char* argv[])
{

        int serverSocket;
        struct sockaddr_in serverAddr;
        struct hostent *hp;

        unsigned short serverPort;
        char* str_port; //[6];
        char* username; //[13];
        char* url; //[17];



        if(argc < 3 || argc > 5)
        {
                fprintf(stderr, "Usage %s serverIP USERNAME [-p PORT]\n", argv[0]);
                exit(1);
        }

        if(argc == 5)
        {
   //             strncpy(str_port, argv[4], 5);
                str_port = argv[4];
                // Convert Port from string to short
        char *ptr;
        serverPort = strtol(str_port, &ptr, 10);

        }
        else
        {
               serverPort = 8371;
        }

 //       strncpy(username, argv[2], 12);
 //       strncpy(url, argv[1], 16);
        username = argv[2];
        url = argv[1];

        printf("USERNAME = %s\n", username);
        printf("URL = %s\n", url);
        printf("Port = %d\n", serverPort);

        
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
        else
        {
                printf("Connection Established\n");
        } 

        int ex = 0;
        char userinput[2030];
        memset(userinput, '\0', sizeof(userinput));

        printf("Userinput blank: %s", userinput);

        char message_to_send[2048];

        memset(userinput, '\0', sizeof(userinput));

	pthread_t thread;
	pthread_create(&thread , NULL , readThread , (void*) &serverSocket);

        while(!ex)
        {
                fgets(userinput, sizeof(userinput), stdin); 

                if(strcmp(userinput, "EXIT\n") == 0)
                {
                        ex = 1;
                }

                message_to_send[0] = '\0';   // ensures the memory is an empty string
                strcat(message_to_send, username);
                strcat(message_to_send, ": ");
                strcat(message_to_send, userinput);
                // Write request to server socket
                if(write(serverSocket, message_to_send, strlen(message_to_send)) < 0)
                {
                        fprintf(stderr, "Write returned an error: %s\n", strerror(errno));
                        exit(1);
                }

        }



        //  free(s);
}


void* readThread(void* val) {
	int socketID = *((int*)val);
	const int MAX_MESSAGE_LENGTH = 2048;
	char buffer[MAX_MESSAGE_LENGTH];
	while(1) {
		bzero(buffer , MAX_MESSAGE_LENGTH);
		read(socketID , buffer , MAX_MESSAGE_LENGTH);
		printf("%s" , buffer);
	}
}
