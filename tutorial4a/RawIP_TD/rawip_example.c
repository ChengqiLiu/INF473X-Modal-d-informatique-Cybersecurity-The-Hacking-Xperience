/*
 * rawip_example.c
 *
 *  Created on: May 4, 2016
 *      Author: jiaziyi
 */


#include<stdio.h>
#include<string.h>
#include<sys/socket.h>
#include<stdlib.h>
#include<netinet/in.h>
#include<arpa/inet.h>

#include "header.h"

// #define SRC_IP  "192.168.1.111" //set your source ip here. It can be a fake one
#define SRC_IP  "10.0.2.15"
#define SRC_PORT 54321 //set the source port here. It can be a fake one

// #define DEST_IP "129.104.89.108" //set your destination ip here
#define DEST_IP "10.0.2.15"
#define DEST_PORT 5555 //set the destination port here
#define TEST_STRING "test data" //a test string as packet payload

int main(int argc, char *argv[])
{
	char source_ip[] = SRC_IP;
	char dest_ip[] = DEST_IP;


	int fd = socket(AF_INET, SOCK_RAW, IPPROTO_RAW);

    int hincl = 1;                  /* 1 = on, 0 = off */
    setsockopt(fd, IPPROTO_IP, IP_HDRINCL, &hincl, sizeof(hincl));

	if(fd < 0)
	{
		perror("Error creating raw socket ");
		exit(1);
	}

	char packet[65536], *data;
	char data_string[] = TEST_STRING;
	memset(packet, 0, 65536);

	//IP header pointer
	struct iphdr *iph = (struct iphdr *)packet;

	//UDP header pointer
	struct udphdr *udph = (struct udphdr *)(packet + sizeof(struct iphdr));
	struct pseudo_udp_header psh; //pseudo header

	//data section pointer
	data = packet + sizeof(struct iphdr) + sizeof(struct udphdr);

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

	//fill the IP header here
	iph->version=4;
	iph->ihl=5;
	iph->tos=0;
	iph->tot_len=sizeof(struct iphdr)+sizeof(struct udphdr)+strlen(data_string);
	iph->id=1234;
	iph->frag_off=0;
	iph->ttl=255;
	iph->protocol=17;
	iph->saddr=src_addr;
	iph->daddr=dst_addr;
	iph->check=checksum((unsigned short*)iph,sizeof(struct iphdr));

	//fill the UDP header
	udph->source=src_port;
	udph->dest=dst_port;
	udph->len=htons(sizeof(struct udphdr)+strlen(data_string));

	//fill the pseudo UDP header
	psh.source_address=src_addr;
    psh.dest_address=dst_addr;
    psh.placeholder=0;
    psh.protocol=IPPROTO_UDP;
    psh.udp_length=udph->len;

	//construct the pseudo UDP datagram
	int pseudo_udp_length=sizeof(struct pseudo_udp_header)+sizeof(struct iphdr)+sizeof(struct udphdr)+strlen(data_string);
    char *pseudo_udp_gram=malloc(pseudo_udp_length);
    memcpy(pseudo_udp_gram,&psh,sizeof(struct pseudo_udp_header));
    memcpy(pseudo_udp_gram+sizeof(struct pseudo_udp_header),packet,sizeof(struct iphdr)+sizeof(struct udphdr)+strlen(data_string));
     
	udph->check=checksum((unsigned short*)pseudo_udp_gram,pseudo_udp_length);

	//send the packet
	struct sockaddr_in sin;
	sin.sin_family=AF_INET;
	sin.sin_port=dst_port;
	sin.sin_addr.s_addr=dst_addr;

	if (sendto (fd,packet,sizeof(struct iphdr)+sizeof(struct udphdr)+strlen(data_string),0,(struct sockaddr*)&sin,sizeof(sin))<0){
        perror("Failed to send.");
    }

	free(pseudo_udp_gram);
	return 0;

}
