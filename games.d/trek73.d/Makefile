#
# Makefile for TREK73
#
ALL	= trek73 bpv shipyard
OPTIONS	= -O #-DTRACE
MODEL	= -i
include ../../CONFIG
#
# Select method of installing the binaries into the system
# NOTE: UNIX System V Release 2.0 does not have install
# and should therefore use either mv or cp.
#
INSTALL = : install

T73OBJECTS = cmds1.o cmds2.o cmds3.o cmds4.o damage.o dist.o endgame.o\
	enemycom.o firing.o globals.o init.o main.o misc.o mission.o\
	moveships.o parseopts.o parsit.o save.o ships.o special.o\
	strat1.o subs.o vers.o

T73CFILES = cmds1.c cmds2.c cmds3.c cmds4.c damage.c dist.c endgame.c\
	enemycom.c firing.c globals.c init.c main.c misc.c mission.c\
	moveships.c parseopts.c pXrsit.c save.c ships.c special.c\
	strat1.c subs.c vers.c

BPVOBJECTS = bpv.o ships.o

BPVFILES = bpv.c ships.c

DYOOBJECTS = shipyard.o

DYOFILES = shipyard.c

HEADS= structs.h defines.h externs.h

RDIST = ${T73CFILES} ${BPVFILES} ${DYOFILES} ${HEADS}

LIBS= -lm

FLUFF = parsit.o make.out errs core lint.errs a.out tags\
	shar.1 shar.2 shar.3 shar.4

trek73:	${T73OBJECTS}
	$(CC) -o $@ ${CFLAGS} ${T73OBJECTS} ${LIBS}

bpv:	${BPVOBJECTS}
	$(CC) -o $@ ${CFLAGS} ${BPVOBJECTS}

shipyard:	${DYOOBJECTS}
	cc -o $@ ${CFLAGS} ${DYOOBJECTS} -lm

install: ${ALL} ${GAMDIR}
	cp ${ALL} ${GAMDIR}
	cd ${GAMDIR}; chmod 711 ${ALL}

${T73OBJECTS}: ${HEADS}

${BPVOBJECTS}: ${HEADS}

${DYOOBJECTS}: ${HEADS}

tags: ${T73CFILES}
	ctags ${T73CFILES} ${HEADS}

count:
	wc ${T73CFILES} ${HEADS}

lint:
	lint -abchx ${CFLAGS} ${T73CFILES} > lint.errs

clean:
	rm -f ${ALL} ${T73OBJECTS} ${DYOOBJECTS} ${BPVOBJECTS} ${FLUFF}

shar: shar.1 shar.2 shar.3 shar.4

shar.1: Makefile defines.h externs.h options.h structs.h cmds1.c cmds2.c
	shar Makefile defines.h externs.h options.h structs.h cmds1.c cmds2.c >$@

shar.2: cmds3.c cmds4.c damage.c dist.c endgame.c enemycom.c
	shar cmds3.c cmds4.c damage.c dist.c endgame.c enemycom.c >$@

shar.3: firing.c globals.c init.c main.c misc.c mission.c bpv.c shipyard.c
	shar firing.c globals.c init.c main.c misc.c mission.c bpv.c shipyard.c >$@

shar.4: moveships.c parseopts.c parsit.c save.c ships.c special.c strat1.c subs.c vers.c 
	shar moveships.c parseopts.c parsit.c save.c ships.c special.c strat1.c subs.c vers.c >$@ 
