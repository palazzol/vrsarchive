#
#	Midway %W% %G%
#
GAMEDIR	= /usr/games
LIBS	= -lcurses -ltermlib 
# Leave in the -O blast you!
CFLAGS	= -O -Mm
FILES	= airstrike.c etc.c midway.c movebombs.c moveships.c screen.c
OBJS	= airstrike.o etc.o midway.o movebombs.o moveships.o screen.o
OTHERF	= makefile externs.h globals.h Midway.doc
JUNKF	= Midway fluff junk arch tags

.c.o:; ${CC} ${CFLAGS} -c $<

Midway: ${OBJS}
	cc ${CFLAGS} ${OBJS} ${LIBS} -o Midway

${OBJS}: externs.h

midway.o: globals.h

arch:
	ar uv MID.a ${FILES} ${OTHERF} 
	touch arch

profile: ${OBJS}
	cc -p -i ${OBJS} ${CFLAGS} ${LIBS} -o Midway.pro

install: ${GAMEDIR} ${GAMEDIR}/Midway ${GAMEDIR}/Midway.doc

${GAMEDIR}:
	mkdir $@
	chmod 755 $@

${GAMEDIR}/Midway: Midway
	cp Midway ${GAMEDIR}/Midway
	chmod 755 ${GAMEDIR}/Midway
	cp /dev/null ${GAMEDIR}/.midwaylog
	chmod 666 ${GAMEDIR}/.midwaylog

${GAMEDIR}/Midway.doc: Midway.doc
	cp Midway.doc ${GAMEDIR}/Midway.doc
	chmod 644 ${GAMEDIR}/Midway.doc

clean:
	rm -f ${OBJS} ${JUNKF}
