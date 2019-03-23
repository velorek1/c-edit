TARGETS = cedit

HEADERS = rawterm.h list_choice.h screen_buffer.h

# The list of object files.
OBJS =  cedit.o rawterm.o list_choice.o screen_buffer.o
OBJS += opfile_dialog.o user_inter.o file_basics.o
OBJS += keyboard.o

# the list of files to clean
TOCLEAN = cedit $(OBJS)

RM ?= rm -f

all: $(TARGETS)
clean:
	$(RM) $(TOCLEAN)

cedit: $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $(OBJS)

# INCLUDE DEPENDENCIES to compile depending on which
# source has been touched.
cedit.o: cedit.c rawterm.h list_choice.h screen_buffer.h keyboard.h opfile_dialog.h user_inter.h file_basics.h
file_basics.o: file_basics.c user_inter.h screen_buffer.h rawterm.h keyboard.h list_choice.h file_basics.h
keyboard.o: keyboard.c rawterm.h keyboard.h
list_choice.o: list_choice.c rawterm.h screen_buffer.h keyboard.h
opfile_dialog.o: opfile_dialog.c screen_buffer.h rawterm.h keyboard.h user_inter.h list_choice.h
rawterm.o: rawterm.c 
screen_buffer.o: screen_buffer.c rawterm.h keyboard.h
user_inter.o: user_inter.c screen_buffer.h rawterm.h keyboard.h list_choice.h user_inter.h
