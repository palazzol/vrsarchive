EXDIR	= /usr/local
MANDIR	= /usr/man/man1
# Use me for system V.
#KEYS	= -DKEY_LEFT="'h'" -DKEY_RIGHT="'l'" -DKEY_UP="'k'" -DKEY_DOWN="'j'"
#CFLAGS	= -O -DSYSV $(KEYS)
#LIB	= -lcurses

#Use me for BSD 4.2
CFLAGS	= -O -DBSD42 -i
LIB	= -lcurses -ltermcap

vc:	sc.o lex.o gram.o interp.o cmds.o
	cc ${CFLAGS} sc.o lex.o gram.o interp.o cmds.o ${LIB} -o vc

diff_to_vc:	diff_to_vc.c
	cc ${CFLAGS} -o dtv diff_to_vc.c

lex.o:	sc.h y.tab.h gram.o

interp.o:	sc.h

sc.o:	sc.h

gram.o:	sc.h y.tab.h

cmds.o: cmds.c sc.h

y.tab.h:	gram.y

gram.o:	gram.y
	yacc -d gram.y;mv y.tab.c gram.c;cc ${CFLAGS} -c gram.c; rm gram.c
	sed<gram.y >experres.h -f eres.sed;sed < gram.y > statres.h -f sres.sed

print:
	(pr gram.y sc.h;cpr interp.c lex.c sc.c) | lpr

clean:
	rm -f *.o *res.h y.tab.h vc vc.old debug core

co: co-all

coall: co-all

co-all:
	co RCS/*

shar:
	shar -a README sc.h interp.c sc.c lex.c cmds.c gram.y eres.sed sres.sed makefile > vc.shar.1 
	shar -a vc.man vc.1 cvt.sed > vc.shar.2

inst-all: inst-vc inst-man

install: $(EXDIR)/vc

inst-man: $(MANDIR)/vc.1

$(EXDIR)/vc: vc
	-mv $(EXDIR)/vc $(EXDIR)/vc.old
	cp vc $(EXDIR)

$(MANDIR)/vc.1: vc.1
	cp vc.1 $(MANDIR)
