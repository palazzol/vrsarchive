#
#	$Header: /home/Vince/cvs/local.d/strings.d/makefile,v 1.1 1987-02-15 16:06:35 vrs Exp $
#
ALL	= strings
MODEL	= -i
OPTIONS	= -O
include ../../CONFIG

install: $(ALL) $(MAN1) $(BINDIR)
	cp $(ALL) $(BINDIR)
	cd $(BINDIR); chmod 711 $(ALL)
	cp strings.1 $(MAN1)
	cd $(MAN1); chmod 644 strings.1

clean:
	rm -f $(ALL)
