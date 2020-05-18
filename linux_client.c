#include <stdio.h>
#include <stdlib.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

int main(int argc, char *argv[]){
	
	//establish client socket
	int client_socket = socket(AF_INET, SOCK_STREAM, 0);
	
	struct in_addr addr;
	//setup address structure
	struct sockaddr_in connect_address;
	connect_address.sin_family = AF_INET;
	connect_address.sin_port = htons(atoi(argv[2]));
	inet_aton(argv[1], (struct in_addr*) &connect_address.sin_addr.s_addr);

	//Connect to server
	connect(client_socket, (struct sockaddr*) &connect_address, sizeof(connect_address));
	char response[256];
	recv(client_socket, &response, sizeof(response), 0);
	printf("%s\n", response);
	close(client_socket);
	return 0;

}


