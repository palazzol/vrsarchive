#
#	$Header: /home/Vince/cvs/local.d/read20.d/makefile,v 1.4 1990-03-30 17:03:19 vrs Exp $
#
ALL	= read20
MODEL	= -i
OPTIONS	= -O
include /etc/CONFIG
CFILES	= read20.c

install: $(ALL)
	cp $(ALL) $(BINDIR)
	cd $(BINDIR); chmod 711 $(ALL)
	cp read20.1 $(MAN1)
	cd $(MAN1); chmod 644 read20.1

read20.o: dump.h

clean:
	rm -f $(ALL)

tags: $(HDRS) $(CFILES)
	ctags $(HDRS) $(CFILES)
