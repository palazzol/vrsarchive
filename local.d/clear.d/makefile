#
#	$Header: /home/Vince/cvs/local.d/clear.d/makefile,v 1.2 1987-02-15 15:49:36 vrs Exp $
#
ALL	= clear
MODEL	= -i
OPTIONS	= -O
include ../../CONFIG

install: $(ALL) $(MAN1) $(BINDIR)
	cp $(ALL) $(BINDIR)
	cd $(BINDIR); chmod 711 $(ALL)
	cp clear.1 $(MAN1)
	cd $(MAN1); chmod 644 clear.1

clean:
	rm -f $(ALL)
