LIBS	= -lcurses -ltermcap
CFLAGS	= -O
GAMEDIR	= $(DESTDIR)/usr/games

SHELL	= /bin/sh
HEADERS	= mille.h
CFILES	= comp.c end.c extern.c init.c mille.c misc.c move.c print.c \
	  roll.c save.c types.c varpush.c
OBJS	= comp.o end.o extern.o init.o mille.o misc.o move.o print.o \
	  roll.o save.o types.o varpush.o
POBJS	= comp.po end.po extern.po init.po mille.po misc.po move.po \
	  roll.po print.po save.po types.po varpush.po

.SUFFIXES: .po .i

.c.po:
	rm -f x.c ; ln $*.c x.c
	${CC} ${CFLAGS} -pg -c x.c
	mv x.o $*.po

.c.i:
	${CC} ${CFLAGS} -P $*.c

mille: ${OBJS} 
	${CC} ${CFLAGS} -o mille ${OBJS} ${LIBS}

${GAMEDIR}:
	mkdir $@
	chmod 755 $@

install: mille ${GAMEDIR}
	cp mille ${GAMEDIR}
	chmod 711 ${GAMEDIR}/mille

pmb: ${POBJS}
	${CC} ${CFLAGS} -pg -o pmb ${POBJS} $(LIBS)

mille.po: mille.c
	rm -f x.c ; ln mille.c x.c
	${CC} ${CFLAGS} -DPROF -p -c x.c
	mv x.o mille.po

table: table.o extern.o
	${CC} ${CFLAGS} -i -o table table.o extern.o

readdump: readdump.o extern.o varpush.o
	${CC} ${CFLAGS} -i -o readdump readdump.o extern.o varpush.o

ctags:
	ctags ${HEADERS} ${CFILES}
	ed - tags < :ctfix
	sort tags -o tags

lint:
	lint -hxb ${CFILES} > lint.out

mille.ar:
	ar ruv mille.ar Makefile tags ${HEADERS} ${CFILES}

tar:
	tar crvf mille.tar Makefile tags :ctfix ${HEADERS} ${CFILES}

lpr:
	pr Makefile ${HEADERS} ${CFILES} tags | lpr ; lpq

clean:
	rm -f ${OBJS} ${POBJS} mille pmb table
