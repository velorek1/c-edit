HEADERS = c_cool.h list_choice.h

default: test1

test1.o: test1.c $(HEADERS)
	gcc -c test1.c 
	gcc -c c_cool.c
	gcc -c list_choice.c

test1: test1.o 
	gcc test1.o c_cool.o list_choice.o -o test1

clean:
	-rm -f test1.o
