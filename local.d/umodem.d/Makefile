#
#	$Header: /home/Vince/cvs/local.d/umodem.d/Makefile,v 1.2 1990-03-30 17:03:43 vrs Exp $
#
ALL	= umodem
MODEL	= -i
OPTIONS	= -O
include /etc/CONFIG

install: $(ALL) $(MAN1) $(BINDIR)
	cp $(ALL) $(BINDIR)
	cd $(BINDIR); chmod 711 $(ALL)

clean:
	rm -f $(ALL)
