SHELL	= /bin/sh
INTERP	=# /usr/games/lib/adv
MAXCODE	= 10000
MAXVOCAB= 1000
COPTS	= -O -i
GAMDIR	= $(DESTDIR)/usr/games
GAMLIB	= $(GAMDIR)/lib
USRMAN	= $(DESTDIR)/usr/man
MANDIR	= $(USRMAN)/man6

CDEFS	= -DDEFINTERPRETER=\"$(INTERP)\" -DMAXCODE=$(MAXCODE)\
	  -DMAXVOCAB=$(MAXVOCAB) -Drindex=strrchr
CFLAGS	= $(COPTS) $(CDEFS)
YFLAGS	= -d
LINTFLGS= -achx $(CDEFS)
LINT	= lint
OBJS	= advgen.o advgram.o advlex.o lookup.o error.o specials.o\
	  vocab.o str.o rooms.o items.o vars.o msgs.o code.o
SRCS	= advgen.c advgram.c advlex.c lookup.c error.c specials.c\
	  vocab.c str.c rooms.c items.c vars.c msgs.c code.c
DIST	= READ_ME TO_DO advgen.6 advgen.c advgram.y advlex.l lookup.c \
	  error.c specials.c vocab.c str.c rooms.c items.c vars.c \
	  msgs.c code.c advgen.h extfuncs.h code.h Makefile Pirate.adv \
	  Dist.hdr

advgen: $(OBJS)
	cc $(COPTS) -o advgen $(OBJS)

lint:	$(SRCS) tokens.h
	$(LINT) $(LINTFLGS) $(SRCS)

$(OBJS):	advgen.h extfuncs.h

advgram.o code.o: code.h

y.tab.h: advgram.c

lookup.o advlex.o: tokens.h

tokens.h: y.tab.h
	-cmp -s y.tab.h tokens.h || cp y.tab.h tokens.h

clean:
	rm -f *.o advgram.c advlex.c y.output y.tab.h tokens.h \
		core Advdist* adv.out advgen

dist:
	rm -f Advdist*
	packmail -hDist.hdr -oAdvdist $(DIST)

$(GAMDIR) $(USRMAN) $(MANDIR):
	mkdir $@
	chmod 755 $@

install: $(GAMDIR) $(GAMLIB) $(USRMAN) $(MANDIR) advgen
	cp advgen $(GAMLIB)
	chmod 711 $(GAMLIB)/advgen
	cp Pirate.adv $(GAMLIB)
	chmod 644 $(GAMLIB)/Pirate.adv
	cp advgen.6 $(MANDIR)
	chmod 644 $(MANDIR)/advgen.6
