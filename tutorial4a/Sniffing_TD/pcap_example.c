/*
 * pcap_example.c
 *
 *  Created on: Apr 28, 2016
 *      Author: jiaziyi
 */


#include<pcap.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>


#include<sys/socket.h>
#include<arpa/inet.h>

#include "pcap_example.h"
#include "header.h"

//some global counter
int tcp=0,udp=0,icmp=0,others=0,igmp=0,total=0,i,j;


int main(int argc, char *argv[])
{
	pcap_t *handle;
	pcap_if_t *all_dev, *dev;

	char err_buf[PCAP_ERRBUF_SIZE], dev_list[30][20];
	char *dev_name;
	bpf_u_int32 net_ip, mask;


	//get all available devices
	if(pcap_findalldevs(&all_dev, err_buf))
	{
		fprintf(stderr, "Unable to find devices: %s", err_buf);
		exit(1);
	}

	if(all_dev == NULL)
	{
		fprintf(stderr, "No device found. Please check that you are running with root \n");
		exit(1);
	}

	printf("Available devices list: \n");
	int c = 1;

	for(dev = all_dev; dev != NULL; dev = dev->next)
	{
		printf("#%d %s : %s \n", c, dev->name, dev->description);
		if(dev->name != NULL)
		{
			strncpy(dev_list[c], dev->name, strlen(dev->name));
		}
		c++;
	}



	printf("Please choose the monitoring device (e.g., en0):\n");
	dev_name = malloc(20);
	fgets(dev_name, 20, stdin);
	*(dev_name + strlen(dev_name) - 1) = '\0'; //the pcap_open_live don't take the last \n in the end

	//look up the chosen device
	int ret = pcap_lookupnet(dev_name, &net_ip, &mask, err_buf);
	if(ret < 0)
	{
		fprintf(stderr, "Error looking up net: %s \n", dev_name);
		exit(1);
	}

	struct sockaddr_in addr;
	addr.sin_addr.s_addr = net_ip;
	char ip_char[100];
	inet_ntop(AF_INET, &(addr.sin_addr), ip_char, 100);
	printf("NET address: %s\n", ip_char);

	addr.sin_addr.s_addr = mask;
	memset(ip_char, 0, 100);
	inet_ntop(AF_INET, &(addr.sin_addr), ip_char, 100);
	printf("Mask: %s\n", ip_char);

	//open the device
	//
	//	   pcap_t *pcap_open_live(char *device,int snaplen, int prmisc,int to_ms,
	//	   char *ebuf)
	//
	//	   snaplen - maximum size of packets to capture in bytes
	//	   promisc - set card in promiscuous mode?
	//	   to_ms   - time to wait for packets in miliseconds before read
	//	   times out
	//	   errbuf  - if something happens, place error string here
	//
	//	   Note if you change "prmisc" param to anything other than zero, you will
	//	   get all packets your device sees, whether they are intendeed for you or
	//	   not!! Be sure you know the rules of the network you are running on
	//	   before you set your card in promiscuous mode!!
	handle = pcap_open_live(dev_name, BUF_SIZE, 1, 0, err_buf);

	if(handle == NULL)
	{
		fprintf(stderr, "Unable to open device %s: %s\n", dev_name, err_buf);
		exit(1);
	}

	printf("Device %s is opened. Begin sniffing...\n", dev_name);

	printf("TCP : %d   UDP : %d   ICMP : %d   IGMP : %d   Others : %d   Total : %d\n", tcp , udp , icmp , igmp , others , total);

	logfile=fopen("log.txt","w");
	if(logfile==NULL)
	{
		printf("Unable to create file.");
	}

	//Captures and prints all DNS queries on all interfaces.  
	char filter_exp[] = "port 53";				// The filter expression
	struct bpf_program fp;						// The compiled filter expression

	if (pcap_compile(handle, &fp, filter_exp, 0, net_ip) == -1){
		fprintf(stderr, "Couldn't parse filter %s: %s\n", filter_exp, pcap_geterr(handle));
		exit(1);
	}

	if (pcap_setfilter(handle, &fp) == -1){
		fprintf(stderr, "Couldn't install filter %s: %s\n", filter_exp, pcap_geterr(handle));
		exit(1);
	}

	//Put the device in sniff loop
	pcap_loop(handle , -1 , process_packet , NULL);

	pcap_close(handle);

	return 0;

}

void process_packet(u_char *args, const struct pcap_pkthdr *header, const u_char *buffer)
{
//	printf("a packet is received! %d \n", total++);
	int size = header->len;

	//Get the IP Header part of this packet , excluding the ethernet header
	struct iphdr *iph = (struct iphdr*)(buffer + sizeof(struct ethhdr));
	++total;

	switch (iph->protocol) //Check the Protocol and do accordingly...
	{
	case 1:  //ICMP Protocol
		++icmp;
		print_icmp_packet( buffer , size);
		break;

	case 2:  //IGMP Protocol
		++igmp;
		break;

	case 6:  //TCP Protocol
		++tcp;
		print_tcp_packet(buffer , size);
		break;

	case 17: //UDP Protocol
		++udp;
		print_udp_packet(buffer , size);
		break;

	default: //Some Other Protocol like ARP etc.
		++others;
		break;
	}
	printf("TCP : %d   UDP : %d   ICMP : %d   IGMP : %d   Others : %d   Total : %d\r", tcp , udp , icmp , igmp , others , total);

}

