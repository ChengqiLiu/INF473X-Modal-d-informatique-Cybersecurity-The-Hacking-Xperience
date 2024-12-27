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

#define INPUT_SIZE 256 //size of input buffer

int main(int argc, char** argv){
    //check number of inputs
    if(argc!=3){
        fprintf(stderr, "The number of parameters should be 3, not %d\n", argc);
        return -1;
    }

    //get input address
    char IP_ADDRESS[16]="";
    if(sscanf(argv[1],"%s",IP_ADDRESS)==EOF){
        fprintf(stderr, "Cannot read IP_ADDRESS %s\n", argv[1]);
        return -1;
    }

    //get input port number
    uint16_t PORT_NUMBER=-1;
    if(sscanf(argv[2],"%hd",&PORT_NUMBER)==EOF){
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
    int ret=inet_pton(AF_INET,(const char *)IP_ADDRESS,(void *)&addr_server.sin_addr.s_addr);
    if(ret<0){
        fprintf(stderr, "Could not identify IP_ADDRESS: %s, %s\n", argv[0], strerror(errno));
        return -1;
    }

    char buffer[INPUT_SIZE];                            //input buffer
    unsigned int msg_len=0;                             //length of input message
    char *msg=(char*)malloc(INPUT_SIZE*sizeof(char));   //record all the input message
    if(msg==NULL){
        fprintf(stderr, "Could not allocate memory: %s, %s\n", argv[0], strerror(errno));
        return -1;
    }
    msg[0]='\0';

    //get input message
    unsigned int i=1;                                   //rounds of using buffer
    while(fgets(buffer,INPUT_SIZE,stdin)!=NULL){
        unsigned int buf_len=strlen(buffer);            //length of the string in input buffer

        //reallocate larger space for msg if necessary
        if(msg_len+buf_len>=i*INPUT_SIZE){
            ++i;

            char* new_ptr=(char*)realloc(msg,i*INPUT_SIZE*sizeof(char));
            if(new_ptr==NULL){
                fprintf(stderr, "Could not allocate memory: %s, %s\n", argv[0], strerror(errno));
                return -1;
            }
            msg=new_ptr;
        }
        msg_len+=buf_len;

        //attach the new input buffer to the end of msg
        strcat(msg,buffer);

        //break at the end
        if(buffer[buf_len]==EOF){
            break;
        }
        
        //send
        if(sendto(socket_fd,(const char *)msg, msg_len, MSG_CONFIRM, (const struct sockaddr *)&addr_server, sizeof(addr_server))<0){
            fprintf(stderr, "Could not send message to the server: %s, %s\n", argv[0], strerror(errno));
            return -1;
        }

        //clear the msg
        msg[0]='\0';
    }

    free(msg);
    return 0;
}