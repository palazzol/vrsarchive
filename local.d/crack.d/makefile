#
#	$Header: /home/Vince/cvs/local.d/crack.d/makefile,v 1.10 2002-11-24 04:59:21 Vincent Exp $
#
ALL	= crack$(EXE)
MODEL	= -i -K
OPTION	= -DTRYEASY
#OPTION	= -DTRYHARD
#OPTION	= -DTRYEASY -DTRYHARD
OPTIONS = -O $(OPTION)
include $(HOME)/CONFIG

crack:	crack.o
	$(CC) $(CFLAGS) -o $@ crack.o $(LIBS)

clean:
	rm -f $(ALL) crack.o

install: $(ALL) $(BINDIR)
	cp crack $(BINDIR)
	chmod 711 $(BINDIR)/crack$(EXE)

crack.o: crack.c primo.h crack.h
