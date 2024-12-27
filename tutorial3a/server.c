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

#define RECV_SIZE 1024      //size of receive buffer
#define NTHREADS 16         //maximum number of threads

int need_new_thread=1;      //1 if all other threads are busy, otherwise 0.
pthread_mutex_t mutex=PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t state_change=PTHREAD_COND_INITIALIZER;

void* receive_send(void*);       //receive and send functions of the server

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

    pthread_t threads[NTHREADS];
    int i;
    //receive
    for(i=0;i<NTHREADS;++i){
        if(pthread_create(&threads[i], NULL, receive_send, (void*)&socket_fd)) {
			fprintf(stderr, "Error creating thread\n");
			return -1;
		}
    }

    //join the NTHREADS threads
	for(i=0;i<NTHREADS;++i){
		pthread_join(threads[i], NULL);
	}

    return 0;
}

void* receive_send(void* socket_fd){
    while(1){
        struct sockaddr_in addr_client;
        unsigned int length=sizeof(addr_client);
        int recv_n;

        //wait if a new thread isn't needed        
        pthread_mutex_lock(&mutex);
        while (need_new_thread==0){
            pthread_cond_wait(&state_change,&mutex);
        }
        need_new_thread=0;
        pthread_mutex_unlock(&mutex);

        //malloc
        char* buffer=(char*)malloc(sizeof(char)*RECV_SIZE);
        
        //reveive
        recv_n=recvfrom(*(int*)socket_fd, buffer, RECV_SIZE, MSG_WAITALL, (struct sockaddr*)&addr_client,&length);
        if(recv_n<=0){
            fprintf(stderr, "Could not receive, %s\n", strerror(errno));
            exit(-1);
        }
        buffer[recv_n]='\0';

        //need a new thread, so send a signal
        pthread_mutex_lock(&mutex);
        need_new_thread=1;
        pthread_cond_signal(&state_change);        
        pthread_mutex_unlock(&mutex);

        //print the received message
        pthread_mutex_lock(&mutex);
        printf("Thread number %ld:\n", (long)pthread_self());
        printf("%s",buffer);
        pthread_mutex_unlock(&mutex);

        //send
        if(sendto(*(int*)socket_fd, (const char *)buffer, recv_n, MSG_CONFIRM, (const struct sockaddr*)&addr_client,sizeof(addr_client))<0){
            fprintf(stderr, "Could not send message to the server, %s\n", strerror(errno));
            exit(-1);
        }

        free(buffer);
    }

    return NULL;
}