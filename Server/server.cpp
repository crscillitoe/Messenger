#include <stdio.h>


int main(int argc, const char *argv[]) {

	short PORT;
	int socketDescriptor;

	if(argc != 2) {
		printf("INVALID SYNTAX\n");
		printf("Valid syntax : %s PORT\n" , argv[0]);
		return -1; //Indicate FAILURE.
	}
}
