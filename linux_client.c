#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <string.h>

#include <openssl/crypto.h>
#include <openssl/x509.h>
#include <openssl/pem.h>
#include <openssl/ssl.h>
#include <openssl/err.h>

int commands(SSL * socket, char * ptr);
char * receive(SSL * socket, FILE *fp); 

int main(int argc, char* argv[]){

	//argument check
	if(argc < 3){
		printf("Usage: client <IP> <port>\n");
		return 1;
	}

	//Initialize socket with arguments
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

	//connect to server
	if (connect(clSoc, claddr->ai_addr, claddr->ai_addrlen)){
		printf("Connect failed\n");
		return 1;
	}
	printf("Connected.\n");
	freeaddrinfo(claddr);

	//OpenSSL
	SSL_library_init();
        OpenSSL_add_all_algorithms();
        SSL_load_error_strings();

        SSL_CTX *ctx = SSL_CTX_new(TLS_client_method());
        if(!ctx){
                fprintf(stderr, "SSL_CTX_new() failed. \n");
                return 1;
        }

	SSL * ssl = SSL_new(ctx);
        if (!ctx){
                fprintf(stderr, "SSL_new() failed.\n");
                return 1;
        }


        SSL_set_fd(ssl, clSoc);
        if (SSL_connect(ssl) == -1) {
                fprintf(stderr, "SSL_connect() failed.\n");
                ERR_print_errors_fp(stderr);
        }



	//while loop to get input
	while(1){


		//FD_set sets input for select. select() polls to find which input (stdin||recv) is ready
		fd_set reads;
		FD_ZERO(&reads);
		FD_SET(clSoc, &reads);
		FD_SET(0, &reads);

		//Timeout value in case there is no input
		struct timeval timeout;
		timeout.tv_sec = 1;
		timeout.tv_usec = 0;
		if (select(clSoc+1, &reads, 0, 0, &timeout) <0){
			printf("select failed");
			return 1;
		}

		//Get information if it is waiting in RECV-Q
		if (FD_ISSET(clSoc, &reads)){
			receive(ssl, stdout);
//			printf("Enter ! followed by Command: (\"Exit\" to quit)\n");
		}

		//Get information entered by the user
		if (FD_ISSET(0, &reads)){
			char read[4096];
			char* ptr = read;
			if (!fgets(read, 4096, stdin))
				break;

			//exit condition
			if(strncmp(ptr+1, "xit\n", 4)==0){
				printf("Exiting connection.\n");
				return 1;
			}
			printf("Sending: %s\n", read);
			int bytes_sent = SSL_write(ssl, read, strlen(read));
			printf("Sent %d bytes.\n", bytes_sent);
			if(commands(ssl, ptr)){
				break;
			}
//			printf("Enter ! followed by Command: (\"Exit\" to quit)\n");
		}
	}
	printf("Closing socket.\n");
	close(clSoc);
	printf("Finished\n");
	return 0;





}


int commands(SSL* socket, char * ptr){
	if(strncmp(ptr, "dl", 2)==0){
		FILE * outfile;
		char * words = strtok(ptr, " ");
		for (int i = 0; i <= 2; i++){
			words = strtok(NULL, " ");
			if (i == 1){
				words = strtok(words, "\n");
			       	outfile = fopen(words, "wb");
				printf("downloading file to %s.\n", words);
			}
		}
		receive(socket, outfile);
	}
	return 0;
}


char * receive(SSL * socket, FILE * fp){
	char read[4096];
	memset(read, 0, sizeof(read));
	int bytes_received = SSL_read(socket, read, 4096);
	int total = bytes_received;
	int i =0;
	if (bytes_received < 1){
		printf("Connection closed by peer.\n");
		exit(1);
		return NULL;
	}
	fwrite(read, bytes_received, 1, fp);
	while (bytes_received == 4096){
		bytes_received = SSL_read(socket, read, 4096);
		total += bytes_received;
//		printf("writing %d to file\n", bytes_received);
		fwrite(read, bytes_received, 1, fp);
	}
	if (fp!=stdout) fclose(fp);
	printf("\nReceived (%d bytes):\n", total);
	printf("Enter ! followed by Command: (\"Exit\" to quit)\n");
	return read;
}
