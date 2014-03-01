all: server/server.o client.o
	g++ -Wall server/server.o -o server/server 
	g++ -Wall client.o -o client
server: server/server.o
	g++ -Wall server/server.c -o server/server.o 
client: client.o
	g++ -Wall client.c -o client.o 
clean: 
	rm -f server/server Server/server.o client client.o
