ALL	= vc vc.1
MODEL	= -i
OPTIONS	= -O
include ../../CONFIG
SRC	= sc.h sc.c lex.c gram.y interp.c cmds.c crypt.c xmalloc.c range.c \
	  eres.sed sres.sed Makefile cvt.sed psc.c
OBJS	= sc.o lex.o gram.o interp.o cmds.o crypt.o xmalloc.o range.o
DOCS	= README sc.doc psc.doc tutorial.sc

gram.o:	sc.h gram.c
	$(CC) ${CFLAGS} -c gram.c

experres.h: gram.c
	sed <gram.y >experres.h -f eres.sed

statres.h: gram.c
	sed <gram.y >statres.h -f sres.sed

y.tab.h gram.c:	gram.y
	yacc -d gram.y
	mv y.tab.c gram.c

lint: sc.h sc.c lex.c gram.c interp.c cmds.c crypt.c
	lint ${CFLAGS} ${SIMPLE} sc.c lex.c gram.c interp.c cmds.c crypt.c \
	range.c xmalloc.c -lcurses -lm

vc:	$(OBJS)
	cc $(CFLAGS) $(OBJS) $(LIBS) -o $@

sc.o:	sc.h
lex.o:	sc.h y.tab.h experres.h statres.h
interp.o:	sc.h
cmds.o: sc.h
crypt.o: sc.h
xmalloc.o: sc.h
range.o: sc.h

vc.1:	sc.doc
	sed -e s/pname/vc/g -e s/PNAME/VC/g sc.doc >$@

vc.man:	vc.1
	nroff -man vc.1 >$@

clean:
	rm -f $(ALL) *.o *res.h y.tab.h debug core

install: $(ALL) $(BINDIR) $(MAN1)
	cp vc $(BINDIR)
	cd $(BINDIR); chmod 711 vc
	cp vc.1 $(MAN1)
	cd $(MAN1); chmod 644 vc.1
