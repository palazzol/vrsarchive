#
#	trek73 %W% %G%
#
SHELL	= /bin/sh
GAMDIR	= $(DESTDIR)/usr/games
USRMAN	= $(DESTDIR)/usr/man
MANDIR	= $(USRMAN)/man6
CFLAGS	= -O -Me -i
LIBS	= -lm
OBJECTS	= cmds1.o cmds2.o cmds3.o cmds4.o dist.o endgame.o enemycom.o\
	  firing.o globals.o main.o misc.o mission.o moveships.o\
	  parseopts.o parsit.o strat1.o subs.o
CFILES	= cmds1.c cmds2.c cmds3.c cmds4.c dist.c endgame.c enemycom.c\
	  firing.c globals.c main.c misc.c mission.c moveships.c\
	  parseopts.c parsit.c strat1.c subs.c
HEADS	= structs.h defines.h

all:	trek73

trek73:	${OBJECTS}
	cc ${CFLAGS} ${OBJECTS} ${LIBS} -o $@

$(GAMDIR) $(USRMAN) $(MANDIR):
	mkdir $@
	chmod 755 $@

install: $(GAMDIR) $(USRMAN) $(MANDIR) trek73
	cp trek73 $(GAMDIR)
	chmod 711 $(GAMDIR)/trek73
	cp trek73.ms $(GAMDIR)
	chmod 644 $(GAMDIR)/trek73.ms
	cp trek73.6 $(MANDIR)
	chmod 644 $(MANDIR)/trek73.6

${OBJECTS}: ${HEADS}

tags: ${CFILES}
	ctags ${CFILES} ${HEADS}

lint:
	lint -za ${CFILES} > lint.out

clean:
	rm -f ${OBJECTS} trek73 make.out core lint.out a.out tags
