#
#	$Header: /home/Vince/cvs/local.d/calend.d/makefile,v 1.4 1987-02-09 06:40:51 vrs Exp $
#
ALL	= calend remind addcal
MODEL	= -F 2000
OPTIONS	= -O
include ../../CONFIG
SRCS	= calend.c process.c util.c parser scanner globals.h process.h pars.h 
DOCS	= calend.1l remind.1l addcal.1l
MISC	= $(DOCS) Makefile remind.c README addcal

all: $(ALL)

calend: calend.o process.o util.o y.tab.o
	cc $(CFLAGS) -o $@ calend.o process.o util.o y.tab.o

calend.c: globals.h process.h

process.c: globals.h process.h

util.c: globals.h

y.tab.c: parser lex.yy.c
	yacc -d parser

lex.yy.c: scanner
	lex scanner

parser: globals.h pars.h

remind: remind.c
	cc $(CFLAGS) -o $@ remind.c

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
