#ifndef NODEH
#define NODEH

#include <stdlib.h>
#include <stdio.h>

typedef struct slnode {
	struct slnode *next;
	void *value; 
} slnode ;

typedef struct sll{
	slnode *head;
	slnode *tail;
	int size;
	void (*display)(FILE *, void *);

} sll;


extern sll *newSLL(void (*displayFunc)(FILE *, void *));
extern void insertSLL(sll *, int , void *);
extern void *removeSLL(sll *, int );
extern void unionSLL(sll *, sll *);
extern int sizeSLL(sll *);
extern void displaySLL(FILE *, sll *);

#endif