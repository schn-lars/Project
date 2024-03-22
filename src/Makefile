all: davis

davis: davis.o
	gcc davis.o -o davis

davis.o: davis.c
	gcc -c davis.c

clean:
	rm -f *.o davis
