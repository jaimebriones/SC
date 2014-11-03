all: exec

exec: cuckoo_search.o RngStream.o functions.o
	gcc RngStream.o cuckoo_search.o functions.o -O2  -o exec -lm -Wall

RngStream.o: RngStream.c RngStream.h
	gcc -c RngStream.c

cuckoo_search.o: cuckoo_search.c cuckoo_search.h
	gcc -c cuckoo_search.c

functions.o: functions.c
	gcc -c functions.c 

clean:
	rm -rf *o exec
