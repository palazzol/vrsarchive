#
#	$Header: /home/Vince/cvs/local.d/calls.d/makefile,v 1.2 1987-02-15 15:49:25 vrs Exp $
#
ALL	= calls
MODEL	= -i
OPTIONS	= -O
include ../../CONFIG

install: $(ALL) $(MAN1) $(BINDIR)
	cp $(ALL) $(BINDIR)
	cd $(BINDIR); chmod 711 $(ALL)
	cp calls.1 $(MAN1)
	cd $(MAN1); chmod 644 calls.1

clean:
	rm -f $(ALL)
