#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/types.h>
#include <sys/socket.h>

#include <netinet/in.h>
#include "cmd.h"

#define BUFSIZE 2048

char* execute(char* cmd[50]){
	char buf[BUFSIZE];
	FILE *fp;

	if ((fp = popen(cmd, "r")) == NULL) {
		printf("Error opening pipe!\n");
		return -1;
	}

	while (fgets(buf, BUFSIZE, fp) != NULL) {
	// Do whatever you want here...
		printf("%s", buf);
		printf("the end?");
	}

	printf("\n\nend\n\n");
/*	if(pclose(fp))  {
		printf("Command not found or exited with error status\n");
		return -1;
	}*/

	return buf;
}

void sendOutput(struct output *start, int socket) {
    struct output * ptr = start;
    while (ptr!=NULL) {
        printf("%s", ptr->line);
	send(socket, ptr->line, ptr->length, 0);
//      printf(" pointer: %p : ", ptr->next);
        ptr = ptr->next;
    }
}


int main(int argc, char* argv[]){

	//Establish socket
	struct output * start;
	int server_sock = socket(AF_INET, SOCK_STREAM, 0);
	struct sockaddr_in server_address;
	server_address.sin_family = AF_INET;

	// Port passed through cmd execution
	printf("Server listening on port %d.\n", atoi(argv[1]));
	server_address.sin_port = htons(atoi(argv[1]));
	server_address.sin_addr.s_addr = INADDR_ANY;

	bind(server_sock, (struct sockaddr*) &server_address, sizeof(server_address));
	listen(server_sock, 5);
	int i = 0;
	int j = 0;
	char message[256] = "The client sent: ";
	char response[BUFSIZE];
	char end[1];
	end[0] = '\0';
	int orig_len = strlen(message);
	printf("\n%d\n",orig_len);
	while(1){
		int client_socket = accept(server_sock, NULL, NULL);
		send(client_socket, "Connected", sizeof("connected"), 0);
		while(1){
			recv(client_socket, &response, sizeof(response), 0);
			if(response[0]=='l'){
				start = call(response);
				sendOutput(start, client_socket);
				send(client_socket, end, 2048, 0);
				freeList(start);
//				send(client_socket, execute(response), 2048, 0);
			}
			else {
				//Copying string 2 to the end of string 1
				for(i=orig_len;response[j]!='\0';i++) 
				  {
				     message[i]=response[j];
				     j++;
				  }
				message[i]='\0';
				if(response[0]=="p"){
					message[20] = response[0];
					message[21] = response[0];
					message[22] = response[0];
				}
				j = 0;
				send(client_socket, message, sizeof(message), 0);
				message[orig_len] ='\0';
			}
			if(!(strncmp("exit", response, 4))){
				printf("exiting");
			}
		}
		close(client_socket);

	}
	return 0;
}
