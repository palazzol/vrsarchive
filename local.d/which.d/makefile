#
#	$Header: /home/Vince/cvs/local.d/which.d/makefile,v 1.2 1990-03-30 17:03:51 vrs Exp $
#
ALL	= which
MODEL	= -i
OPTIONS	= -O
include /etc/CONFIG

install: $(ALL) $(BINDIR) $(MAN1)
	cp $(ALL) $(BINDIR)
	cd $(BINDIR); chmod 711 $(ALL)
	cp which.1 $(MAN1)
	cd $(MAN1); chmod 644 which.1

clean:
	rm -f $(ALL)
