#
#	12/23/85	@(#)Makefile	1.2
#
SHELL	= /bin/sh
GAMDIR	= $(DESTDIR)/usr/games
GAMLIB	= $(GAMDIR)/lib
USRMAN	= $(DESTDIR)/usr/man
MANDIR	= $(USRMAN)/man6
FDEFS	= -DLIBDIR=\"$(GAMLIB)\" -DOBJECTFILE=\"$(GAMLIB)/objects.war\"
CFLAGS	= $(FDEFS) -O -Ml -I ../berkeley.d/h -F 3000
DPYLIB	= ../dpy.d/Llibdpy.a
LIBS	= $(DPYLIB) ../berkeley.d/Llibberkeley.a -ltermlib -lx

CFILES	= main.c talk.c cmd.c view.c init.c scan.c
OFILES	= main.o talk.o cmd.o view.o init.o scan.o

war:		$(OFILES)
		$(CC) $(CFLAGS) -o $@ $(OFILES) $(LIBS)

$(OFILES):	war.h

$(GAMDIR) $(GAMLIB) $(USRMAN) $(MANDIR):
		mkdir $@
		chmod 755 $@

install:	war $(GAMDIR) $(GAMLIB) $(USRMAN) $(MANDIR)
		cp war $(GAMDIR)
		chmod 711 $(GAMDIR)/war
		cp objects.war boards.war $(GAMLIB)
		cd $(GAMLIB); chmod 644 objects.war boards.war
		cp war.6 $(MANDIR)
		chmod 644 $(MANDIR)/war.6

clean:
		rm -f $(OFILES) war
