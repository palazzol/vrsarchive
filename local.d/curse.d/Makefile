#
#	$Header: /home/Vince/cvs/local.d/curse.d/Makefile,v 1.2 1987-02-15 15:52:19 vrs Exp $
#
ALL	= curse
MODEL	= -i -F 2000
OPTIONS	= -O
include ../../CONFIG

install: $(ALL) $(BINDIR) $(MAN1)
	cp $(ALL) $(BINDIR)
	cd $(BINDIR); chmod 711 $(ALL)
	cp curse.1 $(MAN1)
	cd $(MAN1); chmod 644 curse.1

clean:
	rm -f $(ALL)
