#
#	$Header: /home/Vince/cvs/local.d/crack.d/makefile,v 1.8 1988-12-15 15:22:32 vrs Exp $
#
ALL		= crack
MODEL	= -i -K
OPTION = -DTRYEASY
#OPTION	= -DTRYHARD
#OPTION	= -DTRYEASY -DTRYHARD
OPTIONS = -O $(OPTION)
include /etc/CONFIG

crack:	crack.o
	cc $(CFLAGS) -o $@ crack.o

clean:
	rm -f crack crack.o

install: $(ALL) $(BINDIR)
	cp crack $(BINDIR)
	chmod 711 $(BINDIR)/crack

crack.o: crack.c primo.h crack.h
