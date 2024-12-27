/*
 * mutex.c
 *
 *  Created on: Mar 19, 2016
 *      Author: jiaziyi
 */
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>

#define NTHREADS 50
void *increase_counter(void *);

int  counter = 0;
pthread_mutex_t counterMutex=PTHREAD_MUTEX_INITIALIZER;

int main()
{

	//create 50 threads of increase_counter, each thread adding 1 to the counter
	pthread_t threads[NTHREADS];
	int i;
	for(i=0;i<NTHREADS;++i){
		if(pthread_create(&threads[i], NULL, increase_counter, NULL)) {
			fprintf(stderr, "Error creating thread\n");
			return 1;
		}
	}

	//join the 50 threads
	for(i=0;i<NTHREADS;++i){
		pthread_join(threads[i], NULL);
	}

	printf("\nFinal counter value: %d\n", counter);
	return 0;
}

void *increase_counter(void *arg)
{
	pthread_mutex_lock(&counterMutex);

	printf("Thread number %ld, working on counter. The current value is %d\n", (long)pthread_self(), counter);
	int i = counter;
	// usleep (10000); //simulate the data processing
	counter = i+1;
	
	pthread_mutex_unlock(&counterMutex);

	return NULL;
}
