#
#	$Header: /home/Vince/cvs/local.d/crack.d/makefile,v 1.7 1988-11-30 11:38:09 vrs Exp $
#
ALL	= crack
MODEL	= -i -K
OPTIONS	= -O #-DTRYHARD
include /etc/CONFIG

crack:	crack.o
	cc $(CFLAGS) -o $@ crack.o

clean:
	rm -f crack crack.o

install: $(ALL) $(BINDIR)
	cp crack $(BINDIR)
	chmod 711 $(BINDIR)/crack

crack.o: crack.c primo.h crack.h
