#
#	%W%	%G%
#
SHELL	= /bin/sh
SRCS	= calend.c process.c util.c parser scanner globals.h process.h pars.h 
MISC	= calend.1l Makefile remind.c remind.1l README addcal addcal.1l
LOCDIR	= /usr/local
CFLAGS	= -O -F 2000
ScFile	= calend.sc

all: calend remind

calend: calend.o process.o util.o y.tab.o
	cc $(CFLAGS) -o calend calend.o process.o util.o y.tab.o

calend.c: globals.h process.h

process.c: globals.h process.h

util.c: globals.h

y.tab.c: parser lex.yy.c
	yacc -d parser

lex.yy.c: scanner
	lex scanner

parser: globals.h pars.h

remind: remind.c
	cc $(CFLAGS) -o remind remind.c

.DEFAULT:
	co $<

clean:
	rm -f y.* lex.* *.o calend remind core make.out

print:
	cat $(SRCS) | calls >'Function Calls'
	cpr -r 'Function Calls' $(SRCS) Makefile | sprint
	rm 'Function Calls'


$(LOCDIR)/calend: calend
	cp $? $@
	chmod 711 $@

inst-calend: $(LOCDIR)/calend

$(LOCDIR)/addcal: addcal
	cp $? $@
	chmod 755 $@

inst-addcal: $(LOCDIR)/addcal

$(LOCDIR)/remind: remind
	cp $? $@
	chmod 711 $@

inst-remind: $(LOCDIR)/remind

inst-man: /usr/man /usr/man/man1 inst-manc inst-manr inst-mana

/usr/man:
	mkdir $@

/usr/man/man1:
	mkdir $@

inst-manc: /usr/man/man1/calend.1l
inst-manr: /usr/man/man1/remind.1l
inst-mana: /usr/man/man1/addcal.1l

/usr/man/man1/calend.1l: calend.1l
	cp $? $@
	chmod 644 $@

/usr/man/man1/addcal.1l: addcal.1l
	cp $? $@
	chmod 644 $@

/usr/man/man1/remind.1l: remind.1l
	cp $? $@
	chmod 644 $@

install: inst-calend inst-addcal inst-remind inst-man
	@echo Installed
