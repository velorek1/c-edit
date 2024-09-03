.POSIX:
TARGET = cedit
CC     = cc
CFLAGS = -Wall -Wextra -fsigned-char   
LDFLAGS =
LDLIBS  =

# The list of object files.
OBJS =  main.o rterm.o listbox.o scbuf.o
OBJS += ui.o fileb.o global.o menu.o editor.o 
OBJS += keyb.o tm.o edbuf.o opfile.o 

# the list of files to clean
TOCLEAN = cedit $(OBJS)

RM ?= rm -f

all: $(TARGET)
clean:
	$(RM) $(TOCLEAN)

cedit: $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $(OBJS) $(LDLIBS)

# INCLUDE DEPENDENCIES to compile depending on which
# source has been touched.
main.o: main.c rterm.h listbox.h scbuf.h keyb.h ui.h fileb.h tm.h 
fileb.o: fileb.c fileb.h
keyb.o: keyb.c rterm.h keyb.h
listbox.o: listbox.c scbuf.h keyb.h
editor.o: scbuf.h keyb.h rterm.h
rterm.o: rterm.c 
menu.o: listbox.c scbuf.h ui.h keyb.h rterm.h
tm.o: tm.c tm.h
global.o: global.c global.h
scbuf.o: scbuf.c rterm.h keyb.h scbuf.h
ui.o: ui.c scbuf.h rterm.h keyb.h
opfile.o: opfile.c scbuf.h rterm.h listbox.h keyb.h tm.h global.h ui.h 
