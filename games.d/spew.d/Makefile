#
#	$Header: /home/Vince/cvs/games.d/spew.d/Makefile,v 1.3 1990-03-27 17:27:59 vrs Exp $
#
ALL	= spew
OPTIONS = -O -DDEFFILE=\"$(GAMLIB)/headline\"
MODEL	= -i
include /etc/CONFIG

clean:
	rm -f $(ALL)

install: $(ALL)
	cp $(ALL) $(GAMDIR)
	cd $(GAMDIR); chmod 711 $(ALL)
	cp headline $(GAMLIB)
	cd $(GAMLIB); chmod 644 headline
