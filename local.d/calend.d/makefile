#
#	$Header: /home/Vince/cvs/local.d/calend.d/makefile,v 1.7 2002-11-23 07:49:02 Vincent Exp $
#
ALL	= calend remind addcal
MODEL	= -i -F 2000
OPTIONS	= -O
include $(HOME)/CONFIG
SRCS	= calend.c process.c util.c parser scanner globals.h process.h pars.h 
DOCS	= calend.1l remind.1l addcal.1l
MISC	= $(DOCS) Makefile remind.c README addcal

calend: calend.o process.o util.o y.tab.o
	$(CC) $(CFLAGS) -o $@ calend.o process.o util.o y.tab.o

calend.c: globals.h process.h

process.c: globals.h process.h

util.c: globals.h

y.tab.c: parser lex.yy.c
	$(YACC) -d parser

lex.yy.c: scanner
	$(LEX) scanner

parser: globals.h pars.h

remind: remind.c
	$(CC) $(CFLAGS) -o $@ remind.c

clean:
	rm -f y.* lex.* *.o calend remind core make.out

print:
	cat $(SRCS) | calls >'Function Calls'
	cpr -r 'Function Calls' $(SRCS) Makefile | sprint
	rm 'Function Calls'

install: $(ALL) $(BINDIR) $(DOCS) $(MAN1)
	cp $(ALL) $(BINDIR)
	cd $(BINDIR); chmod 711 $(ALL)
	cd $(BINDIR); chmod 755 addcal
	cp $(DOCS) $(MAN1)
	cd $(MAN1); chmod 644 $(DOCS)
