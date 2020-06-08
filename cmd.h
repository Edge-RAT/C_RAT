struct output {
        char* line;
	int length;
        struct output *next;
};
void sendOutput(struct output *start, SSL* socket, int * size);
void handleResponse(char * response, SSL* socket);
struct output *createOutput(size_t length, char *line);
struct output * append(struct output * end, struct output * newOutptr) ;

void printOutput(struct output *start) ;
void returnOutput(struct output *start) ;
void freeList(struct output *start) ;
struct output* file_to_output(FILE* fp, int * size);

