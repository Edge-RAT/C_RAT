#include <stdio.h>
#include <stdlib.h>

#include <sys/types.h>
#include <sys/socket.h>

#include <netinet/in.h>

int main(int argc, char* argv[]){

	//Establish socket
	int server_sock = socket(AF_INET, SOCK_STREAM, 0);
	struct sockaddr_in server_address;
	server_address.sin_family = AF_INET;

	// Port passed through cmd execution
	printf("Server listening on port %d.\n", atoi(argv[1]));
	server_address.sin_port = htons(atoi(argv[1]));
	server_address.sin_addr.s_addr = INADDR_ANY;

	bind(server_sock, (struct sockaddr*) &server_address, sizeof(server_address));
	listen(server_sock, 5);

	while(1){
		int client_socket = accept(server_sock, NULL, NULL);
		send(client_socket, "Connected", sizeof("connected"), 0);
		close(client_socket);
	}

}
