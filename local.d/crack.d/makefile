#
#	$Header: /home/Vince/cvs/local.d/crack.d/makefile,v 1.3 1987-02-09 06:41:08 vrs Exp $
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
	mv crack $(BINDIR)
	chmod 711 $(BINDIR)/crack

crack.o: crack.c crack.h
