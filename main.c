#include "stdio.h"
#include "stdlib.h"
#include "scanner.h"
#include <string.h>
#include "customer.h"
#include <pthread.h>
#include "queue.h"
#include <unistd.h>

pthread_mutex_t lock;
pthread_cond_t mech_cond;
pthread_cond_t oil_cond;
pthread_cond_t time_cond;
pthread_cond_t next_mech_cust;

//Queue for the mechanic lot
int MECHCOUNT = 0;
int OILCOUNT = 0;
int MECHREADY = 0;
int OILREADY = 0;
int TIMECOUNT = 0;
int NUMCUSTOMERS;
int NUMSERVICED;

queue *mechLot;
queue *oilLot;


customer *processCustomer(char *);
void *mechanicProcess();
void *customerProcess(void *);

int main(int argc, char** argv){
	oilLot = newQueue(displayCustomer);
	mechLot = newQueue(displayCustomer);

	pthread_mutex_init(&lock, NULL);
    pthread_cond_init (&mech_cond, NULL);
    pthread_cond_init (&time_cond, NULL);
    pthread_cond_init (&next_mech_cust, NULL);
    pthread_attr_t attr;
    
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
	// //open the jobcard file and read the initial line
	// FILE *jobs = fopen("jobcard", "r");
	// int jobNum = readInt(jobs);
	// int customerCount = 0;
	// //clear the line
	// readLine(jobs);

	// queue *customers = newQueue(displayCustomer);
	// customer *currentCustomer;
	// char *currentLine;
	

	// printf("%d	\n", jobNum);
	// while((currentLine = readLine(jobs))){
	// 	// printf("%s\n", currentLine);
	// 	currentCustomer = processCustomer(currentLine);
	// 	customerCount++;
	// 	enqueue(customers, (void *) currentCustomer);
	// 	displayCustomer(stdout, currentCustomer);
	// }
	// pthread_t threads[customerCount];
	// customer **customerList = (customer **) malloc(sizeof(customer) * customerCount);
	// int cCount = 0;

	// pthread_create(&threads[0], &attr, mechanicProcess, 0);
	// while(sizeQueue(customers)){
	// 	currentCustomer = (customer *) dequeue(customers);
	// 	pthread_create(&threads[0], &attr, customerProcess, (void *) currentCustomer);
	// 	customerList[cCount++] = currentCustomer;
	// }

	// for(int i = 0; i < customerCount; i++){
	// 	pthread_join(threads[i], NULL);
	// }


	customer *c = newCustomer("George", 0, 4, 2);
	customer *c1 = newCustomer("BOB", 1, 5, 2);
	pthread_t mechanic;
	pthread_t notMechanic;
	pthread_t notMechanic2;
	pthread_create(&mechanic, &attr, mechanicProcess, 0);
	pthread_create(&notMechanic, &attr, customerProcess, (void *) c);
	pthread_create(&notMechanic2, &attr, customerProcess, (void *) c1);
	NUMCUSTOMERS = 2;
	pthread_join(notMechanic, 0);
	pthread_join(notMechanic2, 0);

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
	// int count = 0;
	// while(NUMSERVICED < NUMCUSTOMERS){
	// 	//Lock the lock and inform that there's a mechanic ready
	// 	pthread_mutex_lock(&lock);
	// 	MECHREADY++;

	// 	//If there's nothin in the lot we wait
	// 	while(!sizeQueue(mechLot)){
	// 		pthread_cond_wait(&next_mech_cust, &lock);
	// 	}

	// 	//Once there's something in the lot dequeue it and inform that the mech is seeing someone
	// 	pthread_mutex_unlock(&lock);
	// 	customer *servedCustomer = dequeue(mechLot);
	// 	MECHREADY--;
	// 	pthread_mutex_lock(&lock);

	// 	//Iterate time 1 step at a time, locking each time to process arrivals
	// 	while(count < servedCustomer->repairDuration){
	// 		pthread_cond_wait(&time_cond, &lock);
	// 		pthread_mutex_unlock(&lock);
	// 		printf("ADVANCING TIME TO %d\n", TIMECOUNT + 1);
	// 		count++;
	// 		TIMECOUNT++;
	// 		pthread_mutex_lock(&lock);
	// 		//Maybe better to inform everyone on time? Maybe just one
	// 		pthread_cond_broadcast(&time_cond);
	// 	}
	// 	servedCustomer->repairServiced++;
	// 	NUMSERVICED++;
	// 	MECHREADY++;
	// 	pthread_cond_broadcast(&mech_cond);
	// 	pthread_mutex_unlock(&lock);
	// }

	//lock the lock while we assess the BS
	while(1){
		pthread_mutex_lock(&lock);
		MECHREADY++;

	//if we're ready we sleep until we need a customer
		while(!sizeQueue(mechLot)) {
			pthread_mutex_unlock(&lock);
			pthread_cond_signal(&mech_cond);
		}

		customer *current = (customer *) dequeue(mechLot);
		printf("Customer %s SERVICED AT TIME: %d\n", current->name, TIMECOUNT);

		for(int i = 0; i < current->repairDuration; i++) {
			// pthread_mutex_lock(&lock);
			TIMECOUNT++;
			pthread_cond_broadcast(&time_cond);
			printf("INCR TIME TO : %d WITH %s\n", TIMECOUNT, current->name);
			pthread_mutex_unlock(&lock);
			sleep(1);
		}

		pthread_mutex_lock(&lock);
		current->repairServiced++;
		printf("STILL WITH: %s\n", current->name);
		// count++;
		pthread_cond_signal(&next_mech_cust);
		pthread_mutex_unlock(&lock);
	}
	pthread_exit(0);

}

void *customerProcess(void *cust){

	customer *this = (customer *) cust;
	if(this->arrivalTime != TIMECOUNT){
		pthread_cond_wait(&time_cond, &lock);
		printf("%s STARTS SERVICE AT %d\n", this->name, TIMECOUNT);
	}

	enqueue(mechLot, (void *) this);
	
	pthread_cond_signal(&time_cond);
	pthread_mutex_unlock(&lock);

	while(!this->repairServiced) {
		pthread_cond_wait(&next_mech_cust, &lock);
		printf("WERE DONE WITH SERVICE AT TIME: %d\n", TIMECOUNT);
	}
	pthread_mutex_unlock(&lock);
	pthread_exit(0);
}