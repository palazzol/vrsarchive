#
#	Sail - Wooden Ships And Iron Men
#	%W% %G%
#
GAMEDIR		= $(DESTDIR)/usr/games
GAMELIB		= $(GAMEDIR)/lib
SAILMASTER	= cron
DRIVER		= $(GAMELIB)/saildriver
LOGFILE		= $(GAMELIB)/saillog
USRMAN		= $(DESTDIR)/usr/man
MANDIR		= $(USRMAN)/man6
MANDIRS		= $(USRMAN) $(MANDIR)

CTAGS		= ctags -w
CFLAGS		= -i -O
SHELL		= /bin/sh
DFILES		= driver1.c driver2.c driver3.c grap.c boarders.c
DOBJS		= driver1.o driver2.o driver3.o grap.o boarders.o
PFILES		= player1.c player2.c player3.c player4.c player5.c \
		  player6.c player7.c version.c
POBJS		= player1.o player2.o player3.o player4.o player5.o \
		  player6.o player7.o version.o
COMMONFILES	= assorted.c game.c globals.c misc.c parties.c sync.c
COMMONOBJS	= assorted.o game.o globals.o misc.o parties.o sync.o
LFILES		= sail.log.c
LOBJS		= sail.log.o
HDRS		= externs.h player.h driver.h machdep.h
OTHERFILES	= Makefile sail.6
CTAGFILES	= ${DFILES} ${PFILES} ${LFILES} ${COMMONFILES}
JUNKFILES	= make.out core sail driver sail.log tags
PLIBS		= -F 2000 -lcurses -ltermlib
DLIBS		=
LLIBS		=

all: sail driver sail.log

sail: ${POBJS} ${COMMONOBJS}
	cc $(CFLAGS) ${COMMONOBJS} ${POBJS} ${PLIBS} -o sail

driver: ${DOBJS} ${COMMONOBJS}
	cc $(CFLAGS) ${COMMONOBJS} ${DOBJS} ${DLIBS} -o driver

sail.log: ${LOBJS} globals.o
	cc $(CFLAGS) ${LOBJS} globals.o -o sail.log

lint:
	lint -z $(PFILES) $(COMMONFILES) -lcurses
	lint -z $(DFILES) $(COMMONFILES)

tags: ${CTAGFILES}
	${CTAGS} ${CTAGFILES}

${HDRS} ${CTAGFILES}:
	sccs get $@;

clean: 
	rm -f ${DOBJS} ${POBJS} ${LOBJS} ${COMMONOBJS} ${JUNKFILES}

$(GAMEDIR) $(GAMELIB) $(USRMAN) $(MANDIR):
	mkdir $@
	chmod 755 $@

install: sail driver sail.log $(GAMEDIR) $(GAMELIB) $(LOGFILE) $(MANDIRS)
	cp sail sail.log $(GAMEDIR)
	cp driver $(DRIVER)
	chgrp $(SAILMASTER) $(GAMEDIR)/sail $(GAMEDIR)/sail.log
	chgrp $(SAILMASTER) $(DRIVER)
	chown $(SAILMASTER) $(GAMEDIR)/sail $(GAMEDIR)/sail.log
	chown $(SAILMASTER) $(DRIVER)
	chmod 4755 $(GAMEDIR)/sail $(DRIVER)
	chmod  755 $(GAMEDIR)/sail.log
	cp sail.6 $(MANDIR)
	chgrp root $(MANDIR)/sail.6
	chown root $(MANDIR)/sail.6
	chmod  644 $(MANDIR)/sail.6

$(LOGFILE):
	touch ${LOGFILE}
	chown ${SAILMASTER} ${LOGFILE}
	chmod 644 ${LOGFILE}
