#
#	robomatic @(#)Makefile	1.4 10/17/85
#
SHELL	= /bin/sh
LIBS	= -lcurses -ltermcap -lx
GAMDIR	= $(DESTDIR)/usr/games
USRMAN	= $(DESTDIR)/usr/man
MANDIR	= $(USRMAN)/man6

robomatic: robomatic.o
	cc -o $@ robomatic.o $(LIBS)

$(GAMDIR) $(USRMAN) $(MANDIR):
	mkdir $@
	chmod 755 $@

install: robomatic $(GAMDIR) $(USRMAN) $(MANDIR)
	cp robomatic $(GAMDIR)
	chmod 711 $(GAMDIR)/robomatic
	cp robomatic.6 $(MANDIR)
	chmod 711 $(MANDIR)/robomatic.6

lint:
	lint $(LIBS) robomatic.c | tee lint.out

clean:
	rm -f *.o robomatic lint.out core trace.log
