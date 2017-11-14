#ifndef WAITER
#define WAITER
#include "queue.h"
#include "customer.h"

typedef struct waitingLot
{
	queue *lot;
	int capacity;
}waitingLot;


//Create a new waiting lot
waitingLot *newLot(int size);
 
// Let a customer park in the lot
// Returns: 1 if successful, 0 if the lot is full
int parkCustomer(customer *);

#endif