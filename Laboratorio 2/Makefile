FLAGS = -Wall -lm

all: flab2.o fbroker.o fworker.o broker worker lab2

flab2.o:
	gcc -c $(FLAGS) flab2.c

fbroker.o:  
	gcc -c $(FLAGS) fbroker.c
	
fworker.o:
	gcc -c $(FLAGS) fworker.c

broker:
	gcc $(FLAGS) -o broker fbroker.o broker.c 

worker:  
	gcc $(FLAGS) -o worker fworker.o worker.c 
	
lab2: 
	gcc $(FLAGS) -o lab2 flab2.o lab2.c
	
clear: 
	rm -f *.o broker worker lab2
