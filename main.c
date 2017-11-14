#include "stdio.h"
#include "stdlib.h"
#include "scanner.h"
#include <string.h>
#include "customer.h"

customer *processCustomer(char *);

int main(int argc, char** argv){
	
	//open the jobcard file and read the initial line
	FILE *jobs = fopen("jobcard", "r");
	int jobNum = readInt(jobs);

	//clear the line
	readLine(jobs);

	customer *currentCustomer;
	char *currentLine;
	

	printf("%d	\n", jobNum);
	while((currentLine = readLine(jobs))){
		// printf("%s\n", currentLine);
		currentCustomer = processCustomer(currentLine);
		displayCustomer(stdout, currentCustomer);
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

	for(int i = 0; i < strlen(currentLine); i++){
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