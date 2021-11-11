all: work13.o
	gcc -o work13m work13.o

nov1.o: work13.c
	gcc -c work13.c

run:
	./work13m

clean:
	rm *.o
	rm work13m
	rm *.data
