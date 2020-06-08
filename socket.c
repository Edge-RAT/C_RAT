#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <stdio.h>

//establish server socket
int initialize_server(int port) {
	struct sockaddr_in soc;
	int srvSocket = socket(AF_INET, SOCK_STREAM, 0);
	soc.sin_family = AF_INET;
	soc.sin_port = htons(port);
	soc.sin_addr.s_addr = INADDR_ANY;

	if((bind(srvSocket, (struct sockaddr *)&soc, sizeof(soc)))<0){
		printf("Socket bound\n");
		exit(1);
	}
	listen(srvSocket, 10);
	return srvSocket;
}


