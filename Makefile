CC = gcc
CFLAGS = -Wall -Wextra -fmax-errors=10

TARGET = server

Launch: $(TARGET) 
	./$(TARGET) 1337 

$(TARGET): server.o cmd.o
	$(CC) $(CFLAGS) $(TARGET).o cmd.o -o $(TARGET)


$(TARGET).o: linux_server.c cmd.h
	$(CC) $(CFLAGS) -c linux_$(TARGET).c -o $(TARGET).o
	
cmd.o: cmd.c
	$(CC) $(CFLAGS) -c cmd.c -o cmd.o

client: linux_client.c
	$(CC) $(CFLAGS) linux_client.c -o client

clean:
	rm -f $(TARGET) client cmd.o server.o
