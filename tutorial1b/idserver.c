/*
 * idserver.c
 *
 *  Created on: Feb 15, 2016
 *      Author: jiaziyi
 *  Updated: JACF, 2020
 */

#include<stdlib.h>
#include<stdio.h>
#include<string.h>

#include "idserver.h"

/**
 * print the server's information
 */
void print_idserver(idserver s)
{
	/*Print in this format:
		Id: cmok
		atency (usec): 600
		Region: eur
		Status: up
		Nbr of threads: 70*/

	printf("Id: %s\n", s.id);
	printf("Latency (usec): %d\n", s.latency);
	printf("Region: %s\n", s.region);
	printf("Status: %s\n", s.status);
	printf("Nbr of threads: %d\n", *(s.nthreads));
}


/**
 * try to modify the server information
 */
void modify(idserver s, char *id, int latency, char status[])
{
	strncpy(s.id,id,strlen(id));
	s.latency=latency;
	strncpy(s.status,id,strlen(status));
}

/**
 * try to modify the student information using pointer
 */
void modify_by_pointer(idserver *s, char *id, int latency, char status[])
{
	set_str(&(s->id),&(s->id_size),id);
	s->latency=latency;
	set_str(&(s->status),&(s->status_size),status);
}

/**
 * Create a new idserver with initial data.
 */
idserver* create_idserver(char *id, char *region, int latency,
		char *status, int *nthreads)
{
	//Allocate memories.
	idserver* s=(idserver*)malloc(sizeof(idserver));
	initialize_idserver(s);

	//Initialize values.
	modify_by_pointer(s,id,latency,status);
	set_str(&(s->region),&(s->region_size),region);
	*(s->nthreads)=*nthreads;
		
	puts("---print inside create_idserver function---");
	print_idserver(*s);
	puts("---end of print inside");
	return s;
}

/**
 * Initialize an idserver. Allocate memory and initialize data.
 */
void initialize_idserver(idserver *s)
{
	//INitialize the id, region, latency, status, and nthreads.
	s->id=(char*)malloc(SIZE*sizeof(char));
	s->id_size=SIZE*sizeof(char);
	s->id[0]=0;
	
	s->region=(char*)malloc(SIZE*sizeof(char));
	s->region_size=SIZE*sizeof(char);
	s->region[0]='\0';

	s->latency=-1;

	s->status=(char*)malloc(SIZE*sizeof(char));
	s->status_size=SIZE*sizeof(char);
	s->status[0]='\0';

	s->nthreads=(int*)malloc(sizeof(int));
	*(s->nthreads)=0;

}

/**
 * Initialize an idserver *dest by a copy of another idserver *src.
 */
void copy_idserver(idserver *dest, idserver* src)
{
	//Allocate memories.
	initialize_idserver(dest);

	//Initialize values.
	modify_by_pointer(dest, src->id, src->latency, src->status);
	set_str(&(dest->region),&(dest->region_size),src->region);
	*(dest->nthreads)=*(src->nthreads);
}

/**
 * Free the data of an idserver.
 */
void free_idserver(idserver *s)
{
	free(s->id);
	s->id_size=0;

	free(s->region);
	s->region_size=0;

	s->latency=-1;

	free(s->status);
	s->status_size=0;

	free(s->nthreads);
}

/**
 * Copy the contents of string src to dest. Reallocate memory if necessary.
 */
void set_str(char** dest, unsigned int* dest_size, char* src)
{
	unsigned int src_len=strlen(src);
	//Reallocate if necessary.
	while(src_len>=*dest_size){
		free(*dest);
		*dest=(char*)malloc(2*(*dest_size) * sizeof(char));
		(*dest_size)*=2;
	}
	strncpy(*dest,src,src_len+1);
}