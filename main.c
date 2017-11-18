#include "stdio.h"
#include "stdlib.h"
#include "scanner.h"
#include <string.h>
#include "customer.h"
#include <pthread.h>
#include "queue.h"
#include <unistd.h>

pthread_mutex_t lock;
pthread_mutex_t tlock;
pthread_mutex_t oillock;
pthread_mutex_t oiltlock;
pthread_cond_t mech_cond;
pthread_cond_t oil_cond;
pthread_cond_t mech_queue_cond;
pthread_cond_t oil_queue_cond;
pthread_cond_t next_mech_cust;
pthread_cond_t next_oil_cust;

//Queue for the mechanic lot
int MECHCOUNT = 0;
int OILCOUNT = 0;
int MECHREADY = 0;
int OILREADY = 0;
int TIMECOUNT = 0;
int NUMCUSTOMERS;
int NUMSERVICED;
int MAXCUSTOMERS;
int OILCUSTOMERS;

queue *mechLot;
queue *oilLot;


customer *processCustomer(char *);
void *mechanicProcess();
void *customerProcess(void *);
void *oilProcess();

int main(int argc, char** argv){
	oilLot = newQueue(displayCustomer);
	mechLot = newQueue(displayCustomer);

	pthread_mutex_init(&lock, NULL);
	pthread_mutex_init(&tlock, NULL);
	pthread_mutex_init(&oillock, NULL);
	pthread_mutex_init(&oiltlock, NULL);
    pthread_cond_init (&mech_cond, NULL);
    pthread_cond_init (&oil_cond, NULL);
    pthread_cond_init (&mech_queue_cond, NULL);
    pthread_cond_init (&next_mech_cust, NULL);
    pthread_cond_init (&oil_queue_cond, NULL);
    pthread_cond_init (&next_oil_cust, NULL);
    pthread_attr_t attr;
    
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
	//open the jobcard file and read the initial line
	FILE *jobs = fopen("jobcard", "r");
	MAXCUSTOMERS = readInt(jobs);
	int customerCount = 0;
	//clear the line
	readLine(jobs);

	queue *customers = newQueue(displayCustomer);
	customer *currentCustomer;
	char *currentLine;
	
	while((currentLine = readLine(jobs))){
		// printf("%s\n", currentLine);
		currentCustomer = processCustomer(currentLine);
		customerCount++;
		if(currentCustomer->oilDuration > 0) OILCUSTOMERS++;
		enqueue(customers, (void *) currentCustomer);
	}
	pthread_t threads[customerCount];
	pthread_t mechThread;
	pthread_t oilThread;

	customer **customerList = (customer **) malloc(sizeof(customer) * customerCount);
	NUMCUSTOMERS = customerCount;
	int cCount = 0;
	int prevArrival = 0;

	pthread_create(&mechThread, &attr, mechanicProcess, 0);
	pthread_create(&oilThread, &attr, oilProcess, 0);
	while(sizeQueue(customers)){
		currentCustomer = (customer *) dequeue(customers);
		sleep(currentCustomer->arrivalTime - prevArrival);

		pthread_create(&threads[cCount], &attr, customerProcess, (void *) currentCustomer);
		customerList[cCount++] = currentCustomer;
	}

	for(int i = 0; i < customerCount; i++){
		pthread_join(threads[i], NULL);
	}
}

customer *processCustomer(char *currentLine){
	char *name;
	char *current;
	int arrival, repairTime, oilTime;
	int count = 0;
	int aFlag = 0, rFlag = 0, oFlag = 0;

	name = (char *) malloc(sizeof(currentLine));
	current = (char *) malloc(sizeof(currentLine));

	for(int i = 0; i <strlen(currentLine); i++){
			// current[count++] = currentLine[i];
		if(currentLine[i] == ','){
			if(!strlen(name)) {
				strncpy(name, currentLine, i);
				count = i;
			} else if(!aFlag){
				strncpy(current, currentLine + count + 1, i - count - 1);
				arrival = atoi(current);
				count = i;
				aFlag++;
			} else if(!rFlag) {
				strncpy(current, currentLine + count + 1, i - count - 1);
				repairTime = atoi(current);
				count = i;
				rFlag++;
			}
		} else if(i == strlen(currentLine) - 1){
			if(!oFlag) {
				strncpy(current, currentLine + count + 1, i - count);
				oilTime = atoi(current);
				count = i;
				oFlag++;
			}
		}
	}
	return newCustomer(name, arrival, repairTime, oilTime);
}

void *mechanicProcess(){
	customer *current;
	int count = 0;
	while(count < NUMCUSTOMERS){
		//Wait for the lot to have a customer
		pthread_mutex_lock(&lock);
		while(!sizeQueue(mechLot)){
			MECHREADY++;
			pthread_cond_wait(&mech_queue_cond, &lock);
		}
		MECHREADY--;
		current = (customer *) dequeue(mechLot);
		printf("Customer %s - (MC) is being serviced by the mechanic for %d second(s)\n", current->name, current->repairDuration);
		pthread_mutex_unlock(&lock);

		sleep(current->repairDuration);

		pthread_mutex_lock(&lock);
		current->repairServiced++;
		count++;
		MECHREADY++;
		pthread_cond_signal(&mech_cond);
		pthread_mutex_unlock(&lock);
		sleep(1);
	}
	pthread_exit(0);
}

void *customerProcess(void *cust){

	customer *this = (customer *) cust;
	//if the mechanic is sleeping we wake him up
	printf("Customer %s - (MA) arrival \n", this->name);
	pthread_mutex_lock(&lock);
	if(!sizeQueue(mechLot)){
		enqueue(mechLot, (void *) this);
	} else if(sizeQueue(mechLot) == MAXCUSTOMERS) {
		printf("Customer %s - (MZ) leaves busy car maintenance shop\n", this->name);
		pthread_exit(0);
	} 
	else {
		enqueue(mechLot, (void *) this);
	}
	pthread_cond_signal(&mech_queue_cond);
	pthread_mutex_unlock(&lock);
	
	pthread_mutex_lock(&tlock);
	while(!MECHREADY) {
		printf("Customer %s - (MB1) is waiting for mechanic\n", this->name);
		pthread_cond_wait(&next_mech_cust, &tlock);
	}
	pthread_mutex_unlock(&tlock);
 

	pthread_mutex_lock(&lock);
	while(!this->repairServiced) {
		pthread_cond_wait(&mech_cond, &lock);
	}
	printf("Customer %s - (MD) is done with mechanic\n", this->name);
	pthread_mutex_unlock(&lock);

	if(sizeQueue(mechLot)){
		pthread_mutex_lock(&tlock);
		printf("Customer %s - (MC1) notifying customer %s that they are next for mechanic\n", this->name, ((customer *) peekQueue(mechLot))->name);	
		pthread_cond_signal(&next_mech_cust);
		pthread_mutex_unlock(&tlock);
	}

	//Oil Queue Section
	if(this->oilDuration){
		pthread_mutex_lock(&oillock);
		enqueue(oilLot, (void *) this);
		pthread_cond_signal(&oil_queue_cond);
		pthread_mutex_unlock(&oillock);

		pthread_mutex_lock(&oiltlock);
		while(!OILREADY) {
			printf("Customer %s - (MB1) is waiting for oil tech\n", this->name);
			pthread_cond_wait(&next_oil_cust, &oiltlock);
		}
		pthread_mutex_unlock(&oiltlock);

		pthread_mutex_lock(&oillock);
		while(!this->oilServiced) {
			pthread_cond_wait(&oil_cond, &oillock);
		}
		printf("Customer %s - (MD) is done with mechanic\n", this->name);
		pthread_mutex_unlock(&oillock);

		if(sizeQueue(oilLot)){
			pthread_mutex_lock(&oiltlock);
			printf("Customer %s - (MC1) notifying customer %s that they are next for oil tech\n", this->name, ((customer *) peekQueue(oilLot))->name);	
			pthread_cond_signal(&next_oil_cust);
			pthread_mutex_unlock(&oiltlock);
		}
	}
	
	
	printf("Customer %s - (ME) is leaving shop\n", this->name);
	pthread_exit(0);
}

void *oilProcess(){
	customer *current;
	int count = 0;
	while(count < OILCUSTOMERS){
		//Wait for the lot to have a customer
		pthread_mutex_lock(&oillock);
		while(!sizeQueue(oilLot)){
			OILREADY++;
			pthread_cond_wait(&oil_queue_cond, &oillock);
		}
		OILREADY--;
		current = (customer *) dequeue(oilLot);
		printf("Customer %s - (MC) is being serviced by the oil change tech for %d second(s)\n", current->name, current->oilDuration);
		pthread_mutex_unlock(&oillock);

		sleep(current->oilDuration);

		pthread_mutex_lock(&oillock);
		current->repairServiced++;
		count++;
		OILREADY++;
		pthread_cond_signal(&oil_cond);
		pthread_mutex_unlock(&oillock);
		sleep(1);
	}
	pthread_exit(0);
}