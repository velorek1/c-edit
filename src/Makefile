HEADERS = rawterm.h list_choice.h screen_buffer.h

OBJS =  cedit.o rawterm.o list_choice.o screen_buffer.o
OBJS += opfile_dialog.o user_inter.o file_basics.o
OBJS += keyboard.o

TOCLEAN = cedit $(OBJS)

cedit: $(OBJS)
	$(CC) $(LDFLAGS) -o $(.TARGET) $(OBJS)
clean:
	$(RM) $(TOCLEAN)

