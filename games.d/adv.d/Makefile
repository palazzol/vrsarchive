SHELL	= /bin/sh
CFLAGS	= -i -O -DCURSED -F 2000 # Use -DCURSED to get the "curses" version
LIB	= -lcurses -lx # LIB=-lcurses or LIB=-lwinlib" for the original
GAMDIR	= $(DESTDIR)/usr/games
GAMLIB	= $(GAMDIR)/lib

OBJS	= main.o alc.o str.o readin.o nxtwrd.o rdr.o init.o\
	  play.o misc.o prt.o file.o disply.o
SRCS	= main.c alc.c str.c readin.c nxtwrd.c rdr.c init.c\
	  play.c misc.c prt.c file.c disply.c

adv:	$(OBJS)
	$(CC) $(CFLAGS) -o adv $(OBJS) $(LIB) -ltermlib

$(OBJS): adv.h

lint:
	lint -h adv.h $(SRCS)
	
list:
	llist -t adv DOC adv.h $(SRCS)

clean:
	rm -f core *.o adv

$(GAMDIR) $(GAMLIB):
	mkdir $@
	chmod 755 $@

install: adv $(GAMDIR) $(GAMLIB)
	cp adv $(GAMLIB)
	chmod 711 $(GAMLIB)/adv
	cp al p $(GAMLIB)
	chmod 644 $(GAMLIB)/al $(GAMLIB)/p
	cp aland.sh $(GAMDIR)/aland
	chmod 755 $(GAMDIR)/aland
	cp pirate.sh $(GAMDIR)/pirate
	chmod 755 $(GAMDIR)/pirate
