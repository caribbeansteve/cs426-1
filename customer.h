#ifndef CUST
#define CUST
#include "queue.h"
#include "customer.h"
#include <pthread.h>

typedef struct cust
{
	char *name;
	int oilDuration;
	int repairDuration;
	int arrivalTime;
	int repairServiced;
	int oilServiced;
} customer;


customer *newCustomer(char *, int , int , int );
void displayCustomer(FILE *, void *);

#endif