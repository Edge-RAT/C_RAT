#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/types.h>
#include <sys/socket.h>

#include <netinet/in.h>
#include "cmd.h"
#include "srvsocket.h"

#define BUFSIZE 2048

int main(int argc, char* argv[]){

	//Establish socket
	struct output * start;
	int port = atoi(argv[1]);
	if(argc < 2 || port < 0 || port > 65535){
                printf("Usage: server <port>\n");
                return 1;
        }


	int server_sock = 0;
	
	//Initialize server socket - function in socket.c
	server_sock = initialize_server(port);
	printf("Server listening on port %d.\n", port);

	char response[BUFSIZE];
	int client_socket;
	int max_fd;
	int i;
	i = 0;
	max_fd = server_sock;
	fd_set master;
	FD_ZERO(&master);
	FD_SET(server_sock, &master);
	while(1){
		fd_set reads;
		FD_ZERO(&reads);
		reads = master;
		select(max_fd+1, &reads, 0, 0, 0);
		if(FD_ISSET(server_sock, &reads)){
			client_socket = accept(server_sock, NULL, NULL);
			send(client_socket, "Connected", sizeof("connected"), 0);
			FD_SET(client_socket, &master);
			if(client_socket > max_fd)
				max_fd = client_socket;
		}
		else{
			for(i = 1; i <= max_fd; i++){
				if(FD_ISSET(i, &reads)){
					int size = 0;
					memset(response, 0, sizeof(response));
					recv(i, &response, sizeof(response), 0);
					printf("This is what was received: %s;", response);
					//Determines what to do with the command - function in cmd.c
					handleResponse(response, i);
				}
			}
		}
	}


/*	while(client_socket = accept(server_sock, NULL, NULL)){
		send(client_socket, "Connected", sizeof("connected"), 0);
		while(1){
			int size = 0;
			memset(response, 0, sizeof(response));
			recv(client_socket, &response, sizeof(response), 0);
			printf("This is what was received: %s;", response);
			//Determines what to do with the command - function in cmd.c
			handleResponse(response, client_socket);
		}
		close(client_socket);

	}
	*/
	return 0;
}
