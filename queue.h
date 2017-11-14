#ifndef QUEUE_H
#define QUEUE_H
#include "sll.h"

typedef struct queue{
	sll *Queue;
} queue;

extern queue *newQueue(void (*d)(FILE *,void *));
extern void enqueue(queue *items,void *value);
extern void *dequeue(queue *items);
extern void *peekQueue(queue *items);
extern void displayQueue(FILE *fp,queue *items);
extern int sizeQueue(queue *);
#endif
