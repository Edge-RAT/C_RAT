struct output {
        char* line;
	int length;
        struct output *next;
};
struct output *createOutput(int length, char *line);
struct output * append(struct output * end, struct output * newOutptr) ;
void printOutput(struct output *start) ;
void returnOutput(struct output *start) ;
void freeList(struct output *start) ;
struct digit* call(char *cmd);
