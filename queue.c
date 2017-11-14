#include "queue.h"

queue *newQueue(void (*d)(FILE *,void *)){
	queue *newQ = (queue *)malloc(sizeof(queue));
	newQ->Queue = newSLL(d);
	return newQ;
}
void enqueue(queue *this,void *value){
	if(!this->Queue->size){
		insertSLL(this->Queue, 0, value);
		return;
	}
	insertSLL(this->Queue, this->Queue->size, value);
}
void *dequeue(queue *this){
	return removeSLL(this->Queue, 0); 
}
void *peekQueue(queue *this){
	return this->Queue->head->value;
}
void displayQueue(FILE *fp,queue *this){
	displaySLL(fp, this->Queue);
}

int sizeQueue(queue *q){
	return(q->Queue->size);
}