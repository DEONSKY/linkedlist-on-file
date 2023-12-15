CC=gcc

server: 
	$(CC) main.c -o main
	./main

client: 
	$(CC) client.c -o client
	./client
	 