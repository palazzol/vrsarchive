#
#	$Header: /home/Vince/cvs/local.d/vttest.d/makefile,v 1.2 1987-02-15 16:03:42 vrs Exp $
#
ALL	= vttest
MODEL	= -i
OPTIONS	= -O
include ../../CONFIG

install: $(ALL)
	cp $(ALL) $(BINDIR)
	cd $(BINDIR); chmod 711 $(ALL)

clean:
	rm -f $(ALL)
