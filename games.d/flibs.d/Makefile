#
#	flibs %W% %G%
#
SHELL	= /bin/sh
OBJS	= flibs.o fopsys.o fterminal.o
LIBS	= -lcurses -ltermcap -lx
GAMDIR	= $(DESTDIR)/usr/games
USRMAN	= $(DESTDIR)/usr/man
MANDIR	= $(USRMAN)/man6

all:	flibs

flibs:	$(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS) $(LIBS)

$(GAMDIR) $(USRMAN) $(MANDIR):
	mkdir $@
	chmod 755 $@

install: $(GAMDIR) $(USRMAN) $(MANDIR) flibs
	cp flibs $(GAMDIR)
	chmod 711 $(GAMDIR)/flibs
	cp flibs.6 $(MANDIR)
	chmod 644 $(MANDIR)/flibs.6

clean:
	rm -f $(OBJS) flibs flibs.man core

flibs.man: flibs.6
	nroff -man flibs.6 > flibs.man
