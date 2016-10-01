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
#include <arpa/inet.h>
#include <errno.h>
#include <netinet/in.h>
#include <netdb.h>


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



        // Write request to server socket
        if(write(serverSocket, get_request, strlen(get_request)) < 0)
        {
                fprintf(stderr, "Write returned an error: %s\n", strerror(errno));
                exit(1);
        }



        free(s);
}
