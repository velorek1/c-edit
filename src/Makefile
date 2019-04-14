TARGET = cedit

HEADERS = rterm.h listc.h scbuf.h 

# The list of object files.
OBJS =  main.o rterm.o listc.o scbuf.o
OBJS += opfile.o uintf.o fileb.o
OBJS += keyb.o 

# the list of files to clean
TOCLEAN = cedit $(OBJS)

RM ?= rm -f

all: $(TARGET)
clean:
	$(RM) $(TOCLEAN)

cedit: $(OBJS)
	$(CC)  $(LDFLAGS) $(OBJS) -o $(TARGET) 

# INCLUDE DEPENDENCIES to compile depending on which
# source has been touched.
main.o: main.c rterm.h listc.h scbuf.h keyb.h opfile.h uintf.h fileb.h
fileb.o: fileb.c uintf.h fileb.h
keyb.o: keyb.c rterm.h keyb.h
listc.o: listc.c scbuf.h keyb.h
opfile.o: opfile.c scbuf.h rterm.h keyb.h uintf.h
rterm.o: rterm.c 
scbuf.o: scbuf.c rterm.h keyb.h scbuf.h
uintf.o: uintf.c scbuf.h rterm.h keyb.h listc.h uintf.h
