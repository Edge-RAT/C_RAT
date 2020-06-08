#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <openssl/crypto.h>
#include <openssl/x509.h>
#include <openssl/pem.h>
#include <openssl/ssl.h>
#include <openssl/err.h>

#define BUFSIZE 256

//linked list for output to send to client
struct output {
	char* line;
	int length;
	struct output *next;
};

//Create node in linked list
struct output *createOutput(size_t length, char *line){
	int i = 0;
	struct output* ptr;
	ptr = malloc(sizeof(struct output));
	ptr->line = (char*)malloc((length)*sizeof(char));
	memcpy(ptr->line, line, length);
   //     ptr->line[length]='\0';
	ptr->length = length;
	printf("Length:  %ld \n", length);
	for(i = 0; i <=length; i++){
		printf("0x%02x ", ptr->line[i]);
	}
	printf("String in line: %s\n",ptr->line);
	ptr->next = NULL;
	return ptr;
}

//Add a node to the linked list
struct output * append(struct output * end, struct output * newOutptr) {
//	printf("appending\n");
    end->next = newOutptr;

    //DEBUG
//    printf("%s", newOutptr->line);
//    printf(" pointer: %p : ", end->next);
    return(end->next);
    
}

//Print linked list to stdout
void printOutput(struct output *start) {
    struct output * ptr = start;
    while (ptr!=NULL) {
        printf("%s", ptr->line);

	//DEBUG
//	printf(" pointer: %p : ", ptr->next);
        ptr = ptr->next;
    }
}


//void returnOutput(struct output *start) {

//Free up memory space for linked list
void freeList(struct output *start) {
    struct output * ptr = start;
    struct output * tmp;
    while (ptr!=NULL) {
        tmp = ptr->next;
	//free up memory used to store each line of output
//        printf("freeing  pointer: %p : line : %s\n\n", ptr, ptr->line);
	free(ptr->line);
        free(ptr);
//        printf("freed  pointer: %p : line : %s\n\n", ptr, ptr->line);
        ptr = tmp;
    }
}

//Receive command from client and input it to a linked list
struct output* file_to_output(FILE* fp, int * size){
	char buf[BUFSIZE];
	int buffer = 0;
	int length = 0;
	size_t t_BUFSIZE = BUFSIZE;
	size_t bytes_read = BUFSIZE;
	struct output *start, *newOutptr, *end;
//	while (fgets(buf, BUFSIZE, fp) != NULL) {
//              buffer = strlen(buf);
	while (bytes_read == t_BUFSIZE){
		bytes_read = fread(buf, 1, t_BUFSIZE, fp);
		printf("buffer= %ld\n", bytes_read);
		printf("message: %s\n",buf);
                if(length == 0){
                        start = createOutput(bytes_read, buf);
                        end = start;
                }
                else{
                        newOutptr = createOutput(bytes_read, buf);
                        end = append(end, newOutptr);
                }
//		printf("DO I GET HERE?!\n");
                length++;
                *size = *size + (int)bytes_read;
		printf("size: %d", *size);
        }

        if(pclose(fp))  {
                printf("Command not found or exited with error status\n");
                start = NULL;
        }
	printf("start at filetooutput: %p", start);
	return start;
}

//Determine what command the client is requesting
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
	//Shell exec option, prefaced with "!" for example "! ps -ef"
        if(response[0]=='!'){
		if ((fp = popen(ptr+1, "r")) == NULL) {
			printf("Error opening pipe!\n");
			start = NULL;
		}
                start = file_to_output(fp, &size);
                if (start == NULL)
                        printf("error");
                else{
                        printf("%d",size);
                        sendOutput(start, socket, &size);
                        freeList(start);
                }
        }
		//Download file option, prefaced with "dl" for example "dl /etc/passwd"
        else if(strncmp(response, "dl", 2)==0){
			printf("%s", ptr+3);
			if ((fp = fopen( strtok(ptr+3, " "), "rb")) == NULL){
				SSL_write(socket, "File did not open.", sizeof("File did not open."));
				start = NULL;
			}
			else start = file_to_output(fp, &size);
                	if (start == NULL)
                        	printf("error");
	                else{
        	                printf("size = %d\n",size);
                	        sendOutput(start, socket, &size);
                        	freeList(start);
	                }

		}
	else{
                //Copying string 2 to the end of string 1
		//Error check - return string back to the client
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
//        if(!(strncmp("exit", response, 4))){
//                printf("exiting");
//        }
	
}

//Send output to client
void sendOutput(struct output *start, SSL* socket, int * size) {
    struct output * ptr = start;
    char message[*size];
    int prev_length = 0;
    memset(message, 0, sizeof(message));
    while (ptr!=NULL) {
        printf("message!! : %s", ptr->line);
        memcpy(message+prev_length, ptr->line, ptr->length);
	prev_length += ptr->length;
        printf(" pointer: %p : ", ptr->next);
        ptr = ptr->next;
    }   
    printf("\n\nSize of message, %ld: size of size: %d\n\n", sizeof(message),*size);
    SSL_write(socket, message, sizeof(message)); 
    printf("returning to function");
}

/*    int prev_length = 0;
//    char message[*size];
    char * message = malloc(*size * sizeof(char));
    memset(message, 0, sizeof(message));
    while (ptr!=NULL) {
        printf("message!! : %s", ptr->line);
        memcpy(message+prev_length, ptr->line, ptr->length);
        prev_length = ptr->length;
*/
