#
#	$Header: /home/Vince/cvs/local.d/umodem.d/Makefile,v 1.1 1987-02-13 11:16:26 vrs Exp $
#
ALL	= umodem
MODEL	= -i
OPTIONS	= -O
include ../../CONFIG

install: $(ALL) $(MAN1) $(BINDIR)
	cp $(ALL) $(BINDIR)
	cd $(BINDIR); chmod 711 $(ALL)

clean:
	rm -f $(ALL)
