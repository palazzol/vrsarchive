#
#	$Header: /home/Vince/cvs/local.d/copymode.d/makefile,v 1.1 1987-02-15 16:06:15 vrs Exp $
#
ALL	= copymode
MODEL	= -i
OPTIONS	= -O
include ../../CONFIG

install: $(ALL) $(MAN1) $(BINDIR)
	cp $(ALL) $(BINDIR)
	cd $(BINDIR); chmod 711 $(ALL)
	cp copymode.1 $(MAN1)
	cd $(MAN1); chmod 644 copymode.1

clean:
	rm -f $(ALL)
