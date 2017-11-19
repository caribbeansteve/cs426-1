OBJS = scanner.o customer.o queue.o sll.o
FLAGS = gcc -Wall -g -std=c99 -pthread

main: $(OBJS)
	$(FLAGS) main.c -o proj $(OBJS)
customer.o: customer.c customer.h
	$(FLAGS) -c customer.c
scanner.o: scanner.c scanner.h
	$(FLAGS) -c scanner.c
queue.o: queue.c queue.h
	$(FLAGS) -c queue.c
sll.o: sll.c sll.h
	$(FLAGS) -c sll.c
clean:
	rm -f *.o