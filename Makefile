CC = gcc
CFLAGS = -Wall -Wexttra -fmax-errors=10

TARGET = server

Launch: $(TARGET)
	./$(TARGET) 1337 

$(TARGET): linux_server.c
	$(CC) $(CFLAGS) linux_$(TARGET).c -o $(TARGET)

client: linux_client.c
	$(CC) $(CFLAGS) linux_client.c -o client

clean:
	rm -f $(TARGET) client
