#include "sll.h"

extern slnode *makeNode( void *);


//TODO PUT THIS BACK void 
sll *newSLL(void (*displayFunc)(FILE *, void *)){
	sll *newList = (sll *) malloc(sizeof(sll));
	if(!newList){
		fprintf(stderr, "No More Memory\n");
		exit(-1);
	}
	newList->head = 0;
	newList->tail = 0;
	newList->size = 0;
	newList->display = displayFunc;
	return newList;
}


void insertSLL(sll *this, int index, void *item){
	slnode *newNode = makeNode(item);
	slnode *iter = this->head; //For iterating over the linked list
	slnode *temp = this->head;
	int count = 0;

	//Case 1 append to front
	if(!index){
		if(this->size == 0){
			this->tail = newNode;
		} else if(this->size == 1){
			this->tail = this->head;
		}
		newNode->next = temp;
		this->head = newNode;
		this->size++;
	}
	//Case 2 we going back 2 back
	else if(index == this->size) {
		this->tail->next = newNode;
		this->tail = newNode;
		this->size++;
	}
	//Somewhere in the middle 
	else {
		while(count < (index - 1)){
			iter = iter->next;
			count++;
		}
		temp = iter->next;
		newNode->next = temp;
		iter->next = newNode;
		this->size++;
	}
}

void *removeSLL(sll *this, int index){
	int count = 0;
	slnode *iter = this->head;
	void *val;

	//Case 1 Remove from front
	if(!index){
		val = this->head->value;
		this->head = this->head->next;
		this->size--;
	} 
	//Case 2 At the back
	else if(index == this->size){
		while(count < (index - 1)){
			iter = iter->next;
			count++;
		}
		val = iter->next->value;
		this->tail = iter;
		this->size--;
	} 
	//Somewhere in the middle
	else {
		while(count < (index - 1)){
			iter = iter->next;
			count++;
		}
		val = iter->next->value;
		iter->next = iter->next->next;
		this->size--;
	}
	return val;
}

void unionSLL(sll *this, sll *donor){
	if(!this->size){
		this->head = donor->head;
		this->tail = donor->tail;
		this->size = donor->size;
		donor->head = 0;
		donor->tail = 0;
		donor->size = 0;
		return;
	} else if(!donor->size){
		return;
	}
	this->tail->next = donor->head;
	this->tail = donor->tail;
	this->size += donor->size;

	donor->head = 0;
	donor->tail = 0;
	donor->size = 0;
}

int sizeSLL(sll *this){
	return this->size;
}

void displaySLL(FILE *fp, sll *this){
	int count = 0;
	slnode *iter = this->head;
	fprintf(fp, "[");
	while(count < this->size){
		//this->display(out, iter->value);
		this->display(fp, iter->value);
		count++;
		if(count != this->size){
			fprintf(fp, ",");
		}
		iter = iter->next;
	}
	fprintf(fp, "]");
}

slnode *makeNode(void *item){
	slnode *newNode = (slnode *) malloc(sizeof(slnode));
	if(!newNode){
		fprintf(stderr, "No More Memory");
		exit(-1);
	}
	newNode->value = item;
	newNode->next = 0;

	return newNode;
}

