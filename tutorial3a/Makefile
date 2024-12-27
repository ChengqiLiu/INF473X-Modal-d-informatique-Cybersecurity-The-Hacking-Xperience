all: client server test-s test-r

clean:
	rm client.o client server.o server test_continuous_send test_continuous_respond

client.o: client.c
	cc -c -Wall -g client.c

client: client.o
	cc -g -lpthread -o client client.o
	
clean-client:
	rm client.o client

server.o: server.c
	cc -c -Wall -g server.c

server: server.o
	cc -g -lpthread -o server server.o

clean-server:
	rm server.o server

test-s:
	cc test_continuous_send.c -Wall -g -o test_continuous_send

clean-test-s:
	rm test_continuous_send

test-r:
	cc test_continuous_respond.c -Wall -g -o test_continuous_respond

clean-test-r:
	rm test_continuous_respond
