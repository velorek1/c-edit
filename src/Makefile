HEADERS = c_cool.h list_choice.h screen_buffer.h
OBJECTS = cedit.o c_cool.o list_choice.o screen_buffer.o opfile_dialog.o

CFLAGS += -Wall

default: cedit

cedit: $(OBJECTS)
	$(CC) $(LDFLAGS) -o cedit $(OBJECTS) $(LDLIBS)

clean:
	rm -f $(OBJECTS) cedit

$(OBJECTS): $(HEADERS)
