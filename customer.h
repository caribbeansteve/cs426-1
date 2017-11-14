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
} customer;


customer *newCustomer(char *, int , int , int );
void displayCustomer(FILE *, customer *);

#endif