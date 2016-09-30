#include <stdio.h>


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


}
