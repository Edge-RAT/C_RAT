#include <stdio.h>
#include <string.h>
#include <stdlib.h>
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

struct digit* call(char *cmd){
	char buf[BUFSIZE];
	char ** array;
	FILE *fp;
	int buffer = 0;
	int length = 0;
	struct digit *start, *newOutptr, *end, *tmp;
	/*printf("cmd received: ");
	fputs(cmd, stdout);
	sleep(10);
	printf(";");*/
	if ((fp = popen(cmd, "r")) == NULL) {
		printf("Error opening pipe!\n");
		return -1;
	}

	while (fgets(buf, BUFSIZE, fp) != NULL) {
		for(buffer = 0; buf[buffer] !='\0'; buffer++){
			buffer++;
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
	}

	if(pclose(fp))  {
		printf("Command not found or exited with error status\n");
		return -1;
	}
	return start;
}

/*
int main(void){
	char cmd[50];
	struct digit* start;
	while(strncmp("exit", cmd, 4)!=0){
		printf("\n\ncmd: %s\n");	
		fgets(cmd, 50, stdin);
		start = call(cmd);
		printOutput(start);
		freeList(start);
		printf("next? \n");
	}	
}*/
