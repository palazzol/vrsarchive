#
#	$Header: /home/Vince/cvs/games.d/advgen.d/Makefile,v 1.3 1987-12-26 16:53:57 vrs Exp $
#
ALL	= advgen
MODEL	= -i
OPTIONS	= -DDEFINTERPRETER=\"$(INTERP)\" -DMAXCODE=$(MAXCODE) \
	  -DMAXVOCAB=$(MAXVOCAB) -O
include ../../CONFIG
INTERP	= #$(GAMLIB)/adv
MAXCODE	= 10000
MAXVOCAB= 1000
YFLAGS	= -d
LINTFLGS= -achx $(CFLAGS)
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
	$(CC) $(CFLAGS) -o $@ $(OBJS) $(LIBS)

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

install: $(ALL) $(GAMLIB) $(MAN6)
	cp $(ALL) $(GAMLIB)
	cd $(GAMLIB); chmod 711 $(ALL)
	cp Pirate.adv $(GAMLIB)
	chmod 644 $(GAMLIB)/Pirate.adv
	cp advgen.6 $(MAN6)
	chmod 644 $(MAN6)/advgen.6
