#
#	$Header: /home/Vince/cvs/local.d/diffc.d/makefile,v 1.2 1990-03-30 17:02:43 vrs Exp $
#
MODEL	= -i
OPTIONS	= -O
ALL	= diffc
include /etc/CONFIG

install: $(ALL) $(BINDIR) $(MAN1)
	cp $(ALL) $(BINDIR)
	cd $(BINDIR); chmod 711 $(ALL)
	cp diffc.1 $(MAN1)
	cd $(MAN1); chmod 644 diffc.1

clean:
	rm -f $(ALL)
