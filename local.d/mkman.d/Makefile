#
#	$Header: /home/Vince/cvs/local.d/mkman.d/Makefile,v 1.1 1987-02-10 12:53:20 vrs Exp $
#
ALL	= mkman
MODEL	= -i
OPTIONS	= -O
include ../../CONFIG

install: $(ALL) $(BINDIR)
	cp $(ALL) $(BINDIR)
	cd $(BINDIR); chmod 711 $(ALL)

clean:
	rm -f $(ALL)
