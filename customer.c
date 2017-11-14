#include "customer.h"

customer *newCustomer(char *cname, int arrival, int repair, int oil) {
	customer *newCust = (customer *) malloc(sizeof(customer));
	newCust->name = cname;
	newCust->arrivalTime = arrival;
	newCust->oilDuration = oil;
	newCust->repairDuration  = repair;
	return newCust;
}

void displayCustomer(FILE *fp, customer *c){
	fprintf(fp, "%s, %d, %d, %d\n", c->name, c->arrivalTime, c->repairDuration, c->oilDuration);
}