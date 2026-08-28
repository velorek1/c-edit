.POSIX:

PREFIX ?= /usr/local
BINDIR ?= $(PREFIX)/bin

all:
	$(MAKE) -C src all PREFIX="$(PREFIX)" BINDIR="$(BINDIR)"

clean:
	$(MAKE) -C src clean

install:
	$(MAKE) -C src install PREFIX="$(PREFIX)" BINDIR="$(BINDIR)"

uninstall:
	$(MAKE) -C src uninstall PREFIX="$(PREFIX)" BINDIR="$(BINDIR)"

.PHONY: all clean install uninstall
