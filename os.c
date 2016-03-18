#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>

#define MAX_CUSTOMERS 25

void *customer(void *num);
void *employee(void *);
void randwait(int secs);

sem_t waitingRoom;
sem_t employeecounter;
sem_t employeePillow;
sem_t seatBelt;

int allDone = 0;

int main(int argc, char *argv[]) 
{
		pthread_t btid;
		pthread_t tid[MAX_CUSTOMERS];
		long RandSeed;
		int i, numCustomers, numcounters;
		int Number[MAX_CUSTOMERS];

		if (argc != 3) 
		{
			printf("Use: ./a.out <Num Customers> <Num counters> \n");
			exit(-1);
		}

		numCustomers = atoi(argv[1]);
		numcounters = atoi(argv[2]);

		if (numCustomers > MAX_CUSTOMERS) 
		{
			printf("The maximum number of Customers is %d.\n", MAX_CUSTOMERS);
			exit(-1);
		}
		
		printf("\nrailemployee.c\n\n");
		printf("A solution to the Railway Ticket Counter problem using semaphores.\n");


		for (i=0; i<MAX_CUSTOMERS; i++) 
		{
			Number[i] = i;
		}

		sem_init(&waitingRoom, 0, numcounters);
		sem_init(&employeecounter, 0, 1);
		sem_init(&employeePillow, 0, 0);
		sem_init(&seatBelt, 0, 0);

		pthread_create(&btid, NULL, employee, NULL);

		for (i=0; i<numCustomers; i++) 
		{
			pthread_create(&tid[i], NULL, customer, (void *)&Number[i]);
		}

		for (i=0; i<numCustomers; i++) 
		{
			pthread_join(tid[i],NULL);
		}

		allDone = 1;
		sem_post(&employeePillow); // Wake the employee so he will exit.
		pthread_join(btid,NULL);
}

void *customer(void *number) 
{
		int num = *(int *)number;

		printf("Customer %d leaving for employee counter.\n", num);
		randwait(5);

		printf("Customer %d arrived at employee counter.\n", num);
		sem_wait(&waitingRoom);
		
		printf("Customer %d entering waiting room.\n", num);
		sem_wait(&employeecounter);
		sem_post(&waitingRoom);
		
		printf("Customer %d waking the employee.\n", num);
		sem_post(&employeePillow);
		sem_wait(&seatBelt);
		sem_post(&employeecounter);
		
		printf("Customer %d leaving employee counter.\n", num);
}

void *employee(void *junk) 
{
		while (!allDone) 
		{
			printf("The employee is sleeping\n");
			sem_wait(&employeePillow);
			if (!allDone) 
			{
				printf("The employee is booking ticket\n");
				randwait(3);
				printf("The employee has finished booking ticket.\n");
				sem_post(&seatBelt);
			}
			else 
			{
				printf("The employee is going home for the day.\n");
			}
		}
}

void randwait(int secs) 
{
	int len;
	len = (int) ((drand48() * secs) + 1);
	sleep(len);
}
