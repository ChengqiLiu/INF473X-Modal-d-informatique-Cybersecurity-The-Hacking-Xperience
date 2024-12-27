all: client server fancyclient

clean:
	rm client.o client server.o server fancyclient.o fancyclient

client.o: client.c
	cc -c -Wall -g client.c

client: client.o
	cc -g -o client client.o
	
clean-client:
	rm client.o client

server.o: server.c
	cc -c -Wall -g server.c

server: server.o
	cc -g -o server server.o

clean-server:
	rm server.o server

fancyclient.o: fancyclient.c
	cc -c -Wall -g fancyclient.c

fancy: fancyclient.o
	cc -g -o fancyclient fancyclient.o

clean-fancy:
	rm fancyclient.o fancyclient