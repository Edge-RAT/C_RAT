#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/types.h>
#include <sys/socket.h>

#include <netinet/in.h>

#include <openssl/crypto.h>
#include <openssl/x509.h>
#include <openssl/pem.h>
#include <openssl/ssl.h>
#include <openssl/err.h>

#include "cmd.h"
#include "srvsocket.h"

#define BUFSIZE 2048

int main(int argc, char* argv[]){

	struct output * start;
	int port = atoi(argv[1]);

	//handle arguments
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

	//OpenSSL
	SSL_library_init();
	OpenSSL_add_all_algorithms();
	SSL_load_error_strings();

	SSL_CTX *ctx = SSL_CTX_new(TLS_server_method());
	if(!ctx){
		fprintf(stderr, "SSL_CTX_new() failed. \n");
		return 1;
	}

	if (!SSL_CTX_use_certificate_file(ctx, "cert.pem" , SSL_FILETYPE_PEM)|| !SSL_CTX_use_PrivateKey_file(ctx, "key.pem", SSL_FILETYPE_PEM)) {    
		fprintf(stderr, "SSL_CTX_use_certificate_file() failed.\n");    
		ERR_print_errors_fp(stderr);    
		return 1;
	}

	//Array for clinet socket
	SSL * client_SSL[15];
	SSL * ssl;
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
			ssl = SSL_new(ctx);
			if (!ctx){
				fprintf(stderr, "SSL_new() failed.\n");
				return 1;
			}
			SSL_set_fd(ssl, client_socket);
			if (SSL_accept(ssl) <= 0) {
				fprintf(stderr, "SSL_accept() failed.\n");
				ERR_print_errors_fp(stderr);
				return 1;
			}
			client_SSL[client_socket]=ssl;
			printf("SSL connection using %s\n", SSL_get_cipher(ssl));
			SSL_write(ssl, "Connected", sizeof("connected"));
			FD_SET(client_socket, &master);
			if(client_socket > max_fd)
				max_fd = client_socket;
		}
		else{
			for(i = 1; i <= max_fd; i++){
				if(FD_ISSET(i, &reads)){
					int size = 0;
					memset(response, 0, sizeof(response));
					ssl = client_SSL[i];
					if(SSL_read(ssl, &response, sizeof(response))<=0){
						printf("Connection closed\n");
						FD_CLR(i, &master);
					}
					printf("This is what was received: %s;", response);
					//Determines what to do with the command - function in cmd.c
					handleResponse(response, ssl);
				}
			}
		}
	}
	//Client Connection
	SSL_shutdown(ssl);
	close(server_sock);
	SSL_free(ssl);

	//Server Connection
	SSL_CTX_free(ctx);
	return 0;
}
