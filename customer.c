#include "customer.h"

customer *newCustomer(char *cname, int arrival, int repair, int oil) {
	customer *newCust = (customer *) malloc(sizeof(customer));
	newCust->name = cname;
	newCust->arrivalTime = arrival;
	newCust->oilDuration = oil;
	newCust->repairDuration  = repair;
	newCust->repairServiced = 0;
	newCust->oilServiced = 0;
	return newCust;
}

void displayCustomer(FILE *fp, void *cur){
	customer *c = (customer *) cur;
	fprintf(fp, "%s, %d, %d, %d\n", c->name, c->arrivalTime, c->repairDuration, c->oilDuration);
}