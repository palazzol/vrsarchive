#
#	$Header: /home/Vince/cvs/local.d/vttest.d/makefile,v 1.3 1990-03-30 17:03:48 vrs Exp $
#
ALL	= vttest
MODEL	= -i
OPTIONS	= -O
include /etc/CONFIG

install: $(ALL)
	cp $(ALL) $(BINDIR)
	cd $(BINDIR); chmod 711 $(ALL)

clean:
	rm -f $(ALL)
