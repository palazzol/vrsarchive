#
#	$Header: /home/Vince/cvs/local.d/mkman.d/Makefile,v 1.2 1990-03-30 17:03:05 vrs Exp $
#
ALL	= mkman
MODEL	= -i
OPTIONS	= -O
include /etc/CONFIG

install: $(ALL) $(BINDIR)
	cp $(ALL) $(BINDIR)
	cd $(BINDIR); chmod 711 $(ALL)

clean:
	rm -f $(ALL)
