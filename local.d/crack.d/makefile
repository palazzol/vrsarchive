#
#	$Header: /home/Vince/cvs/local.d/crack.d/makefile,v 1.5 1988-11-28 11:32:23 vrs Exp $
#
ALL	= crack
MODEL	= -i -K
OPTIONS	= -O
include /etc/CONFIG

crack:	crack.o
	cc $(CFLAGS) -o $@ crack.o

clean:
	rm -f crack crack.o

install: $(ALL) $(BINDIR)
	cp crack $(BINDIR)
	chmod 711 $(BINDIR)/crack

crack.o: crack.c crack.h
