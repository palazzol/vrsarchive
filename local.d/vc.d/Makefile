ALL	= vc
MODEL	= -i
OPTIONS	= -O
include ../../CONFIG

vc:	sc.o lex.o gram.o interp.o cmds.o
	cc $(CFLAGS) sc.o lex.o gram.o interp.o cmds.o $(LIBS) -o $@

lex.o:	sc.h y.tab.h gram.o

interp.o:	sc.h

sc.o:	sc.h

gram.o:	sc.h y.tab.h

cmds.o: cmds.c sc.h

y.tab.h:	gram.y

gram.o:	gram.y
	yacc -d gram.y;mv y.tab.c gram.c;cc ${CFLAGS} -c gram.c; rm gram.c
	sed<gram.y >experres.h -f eres.sed;sed < gram.y > statres.h -f sres.sed

clean:
	rm -f $(ALL) *.o *res.h y.tab.h debug core

install: $(ALL) $(BINDIR) $(MAN1)
	cp vc $(BINDIR)
	cd $(BINDIR); chmod 711 vc
	cp vc.1 $(MAN1)
	cd $(MAN1); chmod 644 vc.1
