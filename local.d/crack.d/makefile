#
#	$Header: /home/Vince/cvs/local.d/crack.d/makefile,v 1.9 2002-11-23 07:49:03 Vincent Exp $
#
ALL		= crack
MODEL	= -i -K
OPTION = -DTRYEASY
#OPTION	= -DTRYHARD
#OPTION	= -DTRYEASY -DTRYHARD
OPTIONS = -O $(OPTION)
include $(HOME)/CONFIG

crack:	crack.o
	$(CC) $(CFLAGS) -o $@ crack.o $(LIBS)

clean:
	rm -f crack crack.o

install: $(ALL) $(BINDIR)
	cp crack $(BINDIR)
	chmod 711 $(BINDIR)/crack

crack.o: crack.c primo.h crack.h
