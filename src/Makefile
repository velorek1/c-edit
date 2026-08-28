.POSIX:
TARGET = cedit
CC     = cc
CFLAGS = -Wall -Wextra -fsigned-char   
LDFLAGS =
LDLIBS  =

PREFIX ?= /usr/local
BINDIR ?= $(PREFIX)/bin

# The list of object files.
OBJS =  main.o rterm.o listbox.o scbuf.o
OBJS += ui.o fileb.o global.o menu.o editor.o 
OBJS += keyb.o tm.o edbuf.o opfile.o 

# the list of files to clean
TOCLEAN = cedit $(OBJS)

RM ?= rm -f

all: $(TARGET) install

clean:
	$(RM) $(TOCLEAN)

$(TARGET): $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $(OBJS) $(LDLIBS)

install: $(TARGET)
	@mkdir -p $(DESTDIR)$(BINDIR) 2>/dev/null || true
	@if [ -w "$(DESTDIR)$(BINDIR)" ]; then \
		install -m 755 $(TARGET) $(DESTDIR)$(BINDIR)/$(TARGET); \
		echo "=================================================="; \
		echo " [✓] cedit berhasil terpasang ke $(DESTDIR)$(BINDIR)/$(TARGET)"; \
		echo " Sekarang kamu bisa langsung ketik: cedit [file]"; \
		echo "=================================================="; \
	elif [ -n "$$HOME" ] && [ -d "$$HOME/.local/bin" ] || mkdir -p "$$HOME/.local/bin" 2>/dev/null; then \
		install -m 755 $(TARGET) "$$HOME/.local/bin/$(TARGET)"; \
		echo "=================================================="; \
		echo " [✓] cedit berhasil terpasang ke $$HOME/.local/bin/$(TARGET)"; \
		echo " Sekarang kamu bisa langsung ketik: cedit [file]"; \
		echo "=================================================="; \
	else \
		echo "[!] Tidak memiliki izin menulis ke $(DESTDIR)$(BINDIR). Gunakan 'sudo make install' jika perlu."; \
	fi

uninstall:
	$(RM) $(DESTDIR)$(BINDIR)/$(TARGET)
	@if [ -n "$$HOME" ] && [ -f "$$HOME/.local/bin/$(TARGET)" ]; then \
		rm -f "$$HOME/.local/bin/$(TARGET)"; \
	fi
	@echo "[✓] cedit berhasil dicopot."

.PHONY: all clean install uninstall

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
