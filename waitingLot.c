#include "waitingLot.h"

waitingLot *newLot(int size) {
	waitingLot *newWaitingLot = (waitingLot *) malloc(sizeof(waitingLot));
	newWaitingLot->lot = newQueue(displayCustomer);
	newWaitingLot->capacity = size;
}

int parkCustomer(customer *c) {
	if(c->capacity > sizeQueue(c->lot)) {
		enqueue(c->lot, (void *) c);
		return 1;
	}
	return 0;
}