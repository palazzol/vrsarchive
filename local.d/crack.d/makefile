#
#	$Header: /home/Vince/cvs/local.d/crack.d/makefile,v 1.4 1988-01-02 20:59:06 vrs Exp $
#
ALL	= crack
MODEL	= -i -K
OPTIONS	= -O
include ../../CONFIG

crack:	crack.o
	cc $(CFLAGS) -o $@ crack.o

clean:
	rm -f crack crack.o

install: $(ALL) $(BINDIR)
	cp crack $(BINDIR)
	chmod 711 $(BINDIR)/crack

crack.o: crack.c crack.h
