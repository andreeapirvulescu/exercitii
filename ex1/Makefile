build: main.o list.o
	gcc list.o main.o -lpthread -pthread -o main

all:
	gcc -lpthread -pthread -o -g main main.c

list.o: list.c
	gcc -c list.c

clean:
	rm -f *.o main