#
#	$Header: /home/Vince/cvs/local.d/calls.d/makefile,v 1.3 1990-03-30 17:02:26 vrs Exp $
#
ALL	= calls
MODEL	= -i
OPTIONS	= -O
include /etc/CONFIG

install: $(ALL) $(MAN1) $(BINDIR)
	cp $(ALL) $(BINDIR)
	cd $(BINDIR); chmod 711 $(ALL)
	cp calls.1 $(MAN1)
	cd $(MAN1); chmod 644 calls.1

clean:
	rm -f $(ALL)
