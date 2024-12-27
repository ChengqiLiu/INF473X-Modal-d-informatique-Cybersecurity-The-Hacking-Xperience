/*
 * idserver.h
 *
 *  Created on: Feb 15, 2016
 *      Author: jiaziyi
 *  Updated: JACF, 2020
 */

#ifndef IDSERVER_H_
#define IDSERVER_H_
#define SIZE 8 //initial allocated memory size of char*

struct idserver {
	char * id;
	char * region;	// eur asr ame afr
	int latency;	// in usec
	char *status;	// up down unknown
	int *nthreads;

	unsigned int id_size;		//memory size of id
	unsigned int region_size;	//memory size of region
	unsigned int status_size;	//memory size of status
};

typedef struct idserver idserver;

void print_idserver(idserver s);

void modify(idserver s, char *id, int latency, char status[]);

void modify_by_pointer(idserver *s, char *id, int latency, char status[]);

idserver* create_idserver(char *id, char *region, int latency,
		char *status, int *nthreads);
		

void initialize_idserver(idserver *s);

void copy_idserver(idserver *dest, idserver* src);

void free_idserver(idserver *s);

void set_str(char** dest, unsigned int* dest_size, char* src);

#endif /* IDSERVER_H_ */
