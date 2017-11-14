OBJS = scanner.o customer.o
FLAGS = gcc -Wall -g -std=c99 -lpthread

main: $(OBJS)
	$(FLAGS) main.c -o main $(OBJS)
customer.o: customer.c customer.h
	$(FLAGS) -c customer.c
scanner.o: scanner.c scanner.h
	$(FLAGS) -c scanner.c
clean:
	rm -f *.o