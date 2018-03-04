HEADERS = c_cool.h list_choice.h screen_buffer.h

default: cedit

cedit.o: cedit.c $(HEADERS)
	gcc -c -Wall cedit.c 
	gcc -c -Wall c_cool.c
	gcc -c -Wall list_choice.c
	gcc -c -Wall screen_buffer.c

cedit: cedit.o 
	gcc cedit.o c_cool.o list_choice.o screen_buffer.o -o cedit

clean:
	-rm -f cedit.o
