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
#include <pthread.h>

#define INPUT_SIZE 256              //size of input buffer
#define RECV_SIZE 1024              //size of receive buffer

int exit_status=0;                  //If the program completes sending, set to 1.
pthread_mutex_t mutex=PTHREAD_MUTEX_INITIALIZER;

void* send_client(void*);           //send function of the client
void* receive_client(void*);        //receive function of the client

//struct used for the threads
typedef struct client_work{
   int socket_fd;                   //socket
   struct sockaddr_in addr_server;  //address of the server
} client_work_t;                    

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
    client_work_t client_struct;
    client_struct.socket_fd=0;
    client_struct.socket_fd=socket(AF_INET, SOCK_DGRAM, 0);
    if(client_struct.socket_fd<0){
        fprintf(stderr, "Could not create socket: %s\n", strerror(errno));
        return -1;
    }

    //assign values
    client_struct.addr_server.sin_family=AF_INET;
    client_struct.addr_server.sin_port=htons(PORT_NUMBER);
    int ret=inet_pton(AF_INET,(const char *)IP_ADDRESS,(void *)&client_struct.addr_server.sin_addr.s_addr);
    if(ret<0){
        fprintf(stderr, "Could not identify IP_ADDRESS: %s, %s\n", argv[0], strerror(errno));
        return -1;
    }

    pthread_t send_thread;
    pthread_t receive_thread;

    if(pthread_create(&send_thread, NULL, send_client, (void*)&client_struct)) {
		fprintf(stderr, "Error creating thread\n");
		return -1;
	}
    if(pthread_create(&receive_thread, NULL, receive_client, (void*)&client_struct)) {
		fprintf(stderr, "Error creating thread\n");
		return -1;
	}

    pthread_join(send_thread, NULL);
    pthread_join(receive_thread, NULL);
    
    return 0;
}

void* send_client(void* client_struct){
    client_work_t _client_struct=*(client_work_t*)client_struct;

    char buffer[INPUT_SIZE];                            //input buffer
    unsigned int msg_len=0;                             //length of input message
    char *msg=(char*)malloc(INPUT_SIZE*sizeof(char));   //record all the input message
    if(msg==NULL){
        fprintf(stderr, "Could not allocate memory, %s\n", strerror(errno));
        exit(-1);
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
                fprintf(stderr, "Could not allocate memory, %s\n", strerror(errno));
                exit(-1);
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
        if(sendto(_client_struct.socket_fd,(const char *)msg, msg_len, MSG_CONFIRM, (const struct sockaddr *)&_client_struct.addr_server, sizeof(_client_struct.addr_server))<0){
            fprintf(stderr, "Could not send message to the server, %s\n", strerror(errno));
            exit(-1);
        }

        //clear the msg
        msg[0]='\0';
    }

    pthread_mutex_lock(&mutex);
    exit_status=1;
    pthread_mutex_unlock(&mutex);

    free(msg);
    return NULL;
}

void* receive_client(void* client_struct){
    client_work_t _client_struct=*(client_work_t*)client_struct;

    unsigned int length=sizeof(_client_struct.addr_server);
    char recv_buffer[RECV_SIZE];
    int recv_n;

    pthread_mutex_lock(&mutex);
    while (exit_status==0){
        pthread_mutex_unlock(&mutex);

        //receive
        recv_n=recvfrom(_client_struct.socket_fd, (char*)recv_buffer, RECV_SIZE, MSG_WAITALL, (struct sockaddr*)&_client_struct.addr_server,&length);
        if(recv_n<=0){
            fprintf(stderr, "Could not receive, %s\n", strerror(errno));
            exit(-1);
        }
        recv_buffer[recv_n]='\0';

        //print the received message
        printf("%s",recv_buffer);

        pthread_mutex_lock(&mutex);
    }
    pthread_mutex_unlock(&mutex);

    return NULL;
}