#
#	$Header: /home/Vince/cvs/local.d/read20.d/makefile,v 1.2 1987-02-15 15:58:30 vrs Exp $
#
ALL	= read20
MODEL	= -i
OPTIONS	= -O
include ../../CONFIG
CFILES	= read20.c

install: $(ALL)
	cp $(ALL) $(BINDIR)
	cd $(BINDIR); chmod 711 $(ALL)
	cp read20.1 $(MAN1)
	cd $(MAN1); chmod 711 read20.1

read20.o: dump.h

clean:
	rm -f $(ALL)

tags: $(HDRS) $(CFILES)
	ctags $(HDRS) $(CFILES)
