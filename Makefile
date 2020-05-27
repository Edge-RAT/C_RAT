CC = gcc
CFLAGS = -Wall -Wextra -fmax-errors=10

TARGET = server

Launch: $(TARGET) 
	./$(TARGET) 1337 

$(TARGET): server.o cmd.o socket.o client
	$(CC) $(CFLAGS) $(TARGET).o cmd.o socket.o -o $(TARGET)

client: linux_client.c
	$(CC) $(CFLAGS) -c linux_client.c -o client

$(TARGET).o: linux_server.c cmd.h srvsocket.h
	$(CC) $(CFLAGS) -c linux_$(TARGET).c -o $(TARGET).o
	
cmd.o: cmd.c
	$(CC) $(CFLAGS) -c cmd.c -o cmd.o

socket.o: socket.c
	$(CC) $(CFLAGS) -c socket.c -o socket.o

client: linux_client.c
	$(CC) $(CFLAGS) linux_client.c -o client

clean:
	rm -f $(TARGET) client cmd.o server.o socket.o
