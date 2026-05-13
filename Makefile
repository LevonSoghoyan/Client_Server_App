CC = gcc
SERVER_OBJ = server.o
CLIENT_OBJ = client.o
SRC = client.c server.c


server:$(SERVER_OBJ)
	$(CC) $(SERVER_OBJ) -o SERVER
client:$(CLIENT_OBJ)
	$(CC) $(CLIENT_OBJ) -o CLIENT

clean:
	rm -f *.o CLIENT SERVER 
