#
#	Sail - Wooden Ships And Iron Men
#	%W% %G%
#
ALL	= sail driver sail.log
DRIVER	= $(GAMLIB)/saildriver
LOGFILE	= $(GAMLIB)/saillog
MODEL	= -i -F 2000
OPTIONS	= -O -DGAMDIR=\"$(GAMDIR) -DGAMLIB=\"$(GAMLIB)
include /etc/CONFIG

CTAGS	= ctags -w
DFILES	= driver1.c driver2.c driver3.c grap.c boarders.c
DOBJS	= driver1.o driver2.o driver3.o grap.o boarders.o
PFILES	= player1.c player2.c player3.c player4.c player5.c \
	  player6.c player7.c version.c
POBJS	= player1.o player2.o player3.o player4.o player5.o \
	  player6.o player7.o version.o
COMFILES= assorted.c game.c globals.c misc.c parties.c sync.c
COMOBJS	= assorted.o game.o globals.o misc.o parties.o sync.o
LFILES	= sail.log.c
LOBJS	= sail.log.o
HDRS	= externs.h player.h driver.h machdep.h
OTHERFILES= Makefile sail.6
CTAGFILES= ${DFILES} ${PFILES} ${LFILES} ${COMFILES}
JUNKFILES= make.out core sail driver sail.log tags

sail: ${POBJS} ${COMOBJS}
	${CC} $(CFLAGS) ${COMOBJS} ${POBJS} ${LIBS} -o $@

driver: ${DOBJS} ${COMOBJS}
	${CC} $(CFLAGS) ${COMOBJS} ${DOBJS} ${LIBS} -o $@

sail.log: ${LOBJS} globals.o
	${CC} $(CFLAGS) ${LOBJS} globals.o ${LIBS} -o $@

lint:
	lint -z $(PFILES) $(COMFILES) -lcurses
	lint -z $(DFILES) $(COMFILES)

tags: ${CTAGFILES}
	${CTAGS} ${CTAGFILES}

${HDRS} ${CTAGFILES}:
	sccs get $@;

clean: 
	rm -f ${DOBJS} ${POBJS} ${LOBJS} ${COMOBJS} ${JUNKFILES}

install: sail driver sail.log $(GAMDIR) $(GAMLIB) $(LOGFILE) $(MAN6)
	cp sail sail.log $(GAMDIR)
	cp driver $(DRIVER)
	chown $(SAFEUID) $(GAMDIR)/sail $(GAMDIR)/sail.log
	chown $(SAFEUID) $(DRIVER)
	chmod 4755 $(GAMDIR)/sail $(DRIVER)
	chmod  755 $(GAMDIR)/sail.log
	cp sail.6 $(MAN6)
	chgrp root $(MAN6)/sail.6
	chown root $(MAN6)/sail.6
	chmod  644 $(MAN6)/sail.6

$(LOGFILE):
	touch ${LOGFILE}
	chown ${SAFEUID} ${LOGFILE}
	chmod 644 ${LOGFILE}
