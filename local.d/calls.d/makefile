#
#	use 'make' to compile calls
#	use 'make install' to compile and install calls and install the man page
#	use 'make clean' to clean up the directory
#
CFLAGS	= -O -i
DESTDIR	= /usr/local
DOCDIR	= /usr/man/man1
SHELL	= /bin/sh

calls:	calls.o
	$(CC) $(CFLAGS) calls.o -o calls

everything: calls install clean

install: calls $(DOCDIR) $(DESTDIR)
	cp calls $(DESTDIR)/calls
	strip $(DESTDIR)/calls
	cp calls.1 $(DOCDIR)/calls.1

$(DOCDIR) $(DESTDIR):
	mkdir $@

clean:
	rm -f *.o calls make.out core
