HEADERS = rawterm.h list_choice.h screen_buffer.h

default: ./cedit

cedit.o: cedit.c $(HEADERS)
	gcc -c -Wall cedit.c 
	gcc -c -Wall rawterm.c
	gcc -c -Wall list_choice.c
	gcc -c -Wall screen_buffer.c
	gcc -c -Wall opfile_dialog.c
	gcc -c -Wall user_inter.c
	gcc -c -Wall file_basics.c
	gcc -c -Wall keyboard.c

cedit: cedit.o 
	gcc cedit.o rawterm.o keyboard.o list_choice.o screen_buffer.o opfile_dialog.o user_inter.o file_basics.o -o cedit

clean:
	-rm -f cedit.o
