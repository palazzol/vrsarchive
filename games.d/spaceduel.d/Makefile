SHELL	= /bin/sh
CFLAGS	= -i -O -o spaceduel #-DWIZARD
LFLAGS	= -lcurses -ltermlib
GAMDIR	= $(DESTDIR)/usr/games

spaceduel:	spaceduel.c
	${CC} ${CFLAGS} spaceduel.c ${LFLAGS}

install: spaceduel
	cp spaceduel $(GAMDIR)/spaceduel
	chown cron $(GAMDIR)/spaceduel
	chgrp cron $(GAMDIR)/spaceduel
	chmod 4711 $(GAMDIR)/spaceduel
clean:
	rm -f *.o spaceduel core make.out
