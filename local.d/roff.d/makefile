#
#	$Header: /home/Vince/cvs/local.d/roff.d/makefile,v 1.3 1990-03-30 17:03:23 vrs Exp $
#
ALL	= roff
OPTIONS	= -O
include /etc/CONFIG

lint:
	lint roff.c | tee lint.out

install: $(ALL)
	cp $(ALL) $(BINDIR)
	cd $(BINDIR); chmod 711 $(ALL)
	cp roff.1 $(MAN1)
	cd $(MAN1); chmod 644 roff.1

clean:
	rm -f $(ALL) lint.out
