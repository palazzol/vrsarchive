#
#	$Header: /home/Vince/cvs/local.d/strings.d/makefile,v 1.2 1990-03-30 17:03:33 vrs Exp $
#
ALL	= strings
MODEL	= -i
OPTIONS	= -O
include /etc/CONFIG

install: $(ALL) $(MAN1) $(BINDIR)
	cp $(ALL) $(BINDIR)
	cd $(BINDIR); chmod 711 $(ALL)
	cp strings.1 $(MAN1)
	cd $(MAN1); chmod 644 strings.1

clean:
	rm -f $(ALL)
