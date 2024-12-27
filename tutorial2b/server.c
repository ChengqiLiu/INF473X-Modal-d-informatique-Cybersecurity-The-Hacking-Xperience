#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <arpa/inet.h>

#define RECV_SIZE 1024 //size of receive buffer

int main(int argc, char** argv){
    //check number of inputs
    if(argc!=2){
        fprintf(stderr, "The number of parameters should be 2, not %d\n", argc);
        return -1;
    }

    uint16_t PORT_NUMBER=-1;
    if(sscanf(argv[1],"%hd",&PORT_NUMBER)==EOF){
        fprintf(stderr, "Cannot read PORT_NUMBER %s\n", argv[2]);
        return -1;
    }

    //create socket
    int socket_fd=0;
    socket_fd=socket(AF_INET, SOCK_DGRAM, 0);
    if(socket_fd<0){
        fprintf(stderr, "Could not create socket: %s\n", strerror(errno));
        return -1;
    }

    //assign values
    struct sockaddr_in addr_server;
    addr_server.sin_family=AF_INET;
    addr_server.sin_port=htons(PORT_NUMBER);
    addr_server.sin_addr.s_addr=INADDR_ANY;

    //bind the socket to the ports
    if(bind(socket_fd,(const struct sockaddr *)&addr_server,sizeof(addr_server))<0){
		fprintf(stderr, "Could not bind the socket to the port: %s, %s\n", argv[0], strerror(errno));
		return -1;
	}

    struct sockaddr_in addr_client;
    unsigned int length=sizeof(addr_client);
	char buffer[RECV_SIZE];
    int recv_n;

    while(1){
        //reveive
        recv_n=recvfrom(socket_fd, (char*)buffer, RECV_SIZE, MSG_WAITALL, (struct sockaddr*)&addr_client,&length);
        if(recv_n<=0){
            fprintf(stderr, "Could not receive: %s, %s\n", argv[0], strerror(errno));
            return -1;
        }
        buffer[recv_n]='\0';

        //print the received message
        printf("%s",buffer);

        //send
        if(sendto(socket_fd, (const char *)buffer, recv_n, MSG_CONFIRM, (const struct sockaddr*)&addr_client,sizeof(addr_client))<0){
            fprintf(stderr, "Could not send message to the server: %s, %s\n", argv[0], strerror(errno));
            return -1;
        }
    }

    return 0;
}