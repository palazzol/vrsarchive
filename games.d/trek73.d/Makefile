#
# Makefile for TREK73
#
ALL	= trek73 printships shipyard
#
# Options that you should think about setting
#
# SHOWTORP: When set, the player decides whether or not photon
#     torpedos should show up on a position display (command 13)
#     If not set, torpedos will automatically be displayed.
#
# PARANOID: If you want ALL the checking of a save file that rogue
#     provides, set this.  If you want to be cool, don't set it.
#
# NOTDEF: This option only makes sense if you have also set PARANOID.
#     This option defeats making a link to a save file and then
#     restarting the game from the link.
#
# HISTORICAL: If this option is set, then any ship which takes more
#     than 43 points of actual damage would explode.  This led to
#     much abuse of high-pod probes.  For more details, cf damage.c
#
OPTIONS	= -O -DPARSER #-DTRACE
MODEL	= -i
include ../../CONFIG
#
# Select whether or not to include the getopt.o module.
# Generally, AT&T-derived systems will have getopt.o already
# included in libc.  This may not be true of BSD UNIX.
#GETOPT = getopt.o

MAN	= bpv.6 shipyard.6 trek73.6

T73OBJECTS = cmds1.o cmds2.o cmds3.o cmds4.o damage.o dist.o endgame.o\
	enemycom.o firing.o globals.o init.o main.o misc.o mission.o\
	moveships.o parseopts.o save.o ships.o special.o strat1.o\
	subs.o vers.o $(GETOPT)

T73CFILES = cmds1.c cmds2.c cmds3.c cmds4.c damage.c dist.c endgame.c\
	enemycom.c firing.c globals.c init.c main.c misc.c mission.c\
	moveships.c parseopts.c save.c ships.c special.c strat1.c\
	subs.c vers.c

PARSEFILES = command.l grammar.y

PRTOBJECTS = bpv.o ships.o printships.o

PRTFILES = bpv.c ships.c printships.c

DYOOBJECTS = shipyard.o bpv.o

DYOFILES = shipyard.c bpv.c

HEADS= defines.h externs.h structs.h union.h

RDIST = $(T73CFILES) $(PARSEFILES) $(PRTFILES) $(DYOFILES) $(HEADS)


FLUFF = parsit.o lex.yy.c y.tab.c grammar.c grammar.o\
	make.out errs core lint.errs a.out tags
#
# If you are not using the parser, you do not need the lex and yacc libs
#
trek73:	$(T73OBJECTS) grammar.o parsit.o
	$(CC) $(CFLAGS) -o $@ $(T73OBJECTS) grammar.o parsit.o $(LIBS) -ll -ly

printships:	$(PRTOBJECTS)
	$(CC) $(CFLAGS) -o $@ $(PRTOBJECTS) $(LIBS)

shipyard:	$(DYOOBJECTS)
	$(CC) $(CFLAGS) -o $@ $(DYOOBJECTS) $(LIBS)

install: $(ALL) $(GAMDIR) $(MAN6) $(GAMLIB)
	cp $(ALL) $(GAMDIR)
	cd $(GAMDIR); chmod 711 $(ALL)
	cp $(MAN) $(MAN6)
	cd $(MAN6); chmod 644 $(MAN)
	cp trek73.ms $(GAMLIB)
	cd $(GAMLIB); chmod 644 trek73.ms

$(T73OBJECTS): $(HEADS)

$(PRTOBJECTS): $(HEADS)

$(DYOOBJECTS): $(HEADS)

grammar.o: grammar.y union.h command.l
	lex command.l
	yacc grammar.y
	mv y.tab.c grammar.c
	cc -c $(CFLAGS) grammar.c

parsit.o: parsit.c
	cc -c $(CFLAGS) parsit.c

tags: $(T73CFILES)
	ctags $(T73CFILES) $(HEADS)

count:
	wc $(T73CFILES) $(HEADS)

#
# Flags for your lint may need to be changed
#
lint:
	lint -bx $(OPTIONS) $(T73CFILES) > lint.errs

clean:
	rm -f $(ALL) $(T73OBJECTS) $(DYOOBJECTS) $(PRTOBJECTS) $(FLUFF)
