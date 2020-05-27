struct output {
        char* line;
	int length;
        struct output *next;
};
void handleResponse(char * response, int socket);
struct output *createOutput(int length, char *line);
struct output * append(struct output * end, struct output * newOutptr) ;
void printOutput(struct output *start) ;
void returnOutput(struct output *start) ;
void freeList(struct output *start) ;
struct output* call(char *cmd, int * size);
void sendOutput(struct output *start, int socket, int * size);

