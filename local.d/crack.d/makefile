#
#	$Header: /home/Vince/cvs/local.d/crack.d/makefile,v 1.2 1987-02-08 23:26:46 vrs Exp $
#
ALL	= crack
OPTIONS	= -i -O -K
include ../../CONFIG

crack:	crack.o
	cc $(CFLAGS) -o $@ crack.o

clean:
	rm -f crack crack.o

install: $(ALL) $(BINDIR)
	mv crack $(BINDIR)
	chmod 711 $(BINDIR)/crack

crack.o: crack.c crack.h
