#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <string.h>

int main(int argc, char* argv[]){
	if(argc < 3){
		printf("Usage: client <IP> <port>\n");
		return 1;
	}

	struct addrinfo initaddr;
	memset(&initaddr, 0, sizeof(initaddr));
	initaddr.ai_socktype = SOCK_STREAM;
	struct addrinfo *claddr;
	if (getaddrinfo(argv[1], argv[2], &initaddr, &claddr)){
		printf("Initializing socket failed");
		return 1;
	}
	int clSoc;
	clSoc = socket(claddr->ai_family, claddr->ai_socktype, claddr->ai_protocol);
	printf("connecting to: %s:%s\n", argv[1], argv[2]);
	if (connect(clSoc, claddr->ai_addr, claddr->ai_addrlen)){
		printf("Connect failed\n");
		return 1;
	}
	printf("Connected\n");
	freeaddrinfo(claddr);
	while(1){
		fd_set reads;
		FD_ZERO(&reads);
		FD_SET(clSoc, &reads);
		FD_SET(0, &reads);
		struct timeval timeout;
		timeout.tv_sec = 1;
		timeout.tv_usec = 0;
		if (select(clSoc+1, &reads, 0, 0, &timeout) <0){
			printf("select failed");
			return 1;
		}
		if (FD_ISSET(clSoc, &reads)){
			char read[4096];
			int bytes_received = recv(clSoc, read, 4096, 0);
			if (bytes_received < 1){
				printf("Connection closed by peer.\n");
				break;
			}
			printf("Received (%d bytes):\n %.*s\n", bytes_received, bytes_received, read);
		}
		if (FD_ISSET(clSoc, &reads)){
			char read[4096];
			if (!fgets(read, 4096, stdin))
				break;
			if(strncmp(read, "exit", 4)==0){
				printf("Exiting connection.\n");
				break;
			}
			printf("Sending: %s\n", read);
			int bytes_sent = send(clSoc, read, strlen(read), 0);
			printf("Sent %d bytes.\n", bytes_sent);
		}
	}
	printf("Closing socket.\n");
	close(clSoc);
	printf("Finished\n");
	return 0;





}
