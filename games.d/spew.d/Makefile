#
#	$Header: /home/Vince/cvs/games.d/spew.d/Makefile,v 1.2 1987-05-17 19:35:10 vrs Exp $
#
ALL	= spew
OPTIONS = -O -DDEFFILE=\"$(GAMLIB)/headline\"
MODEL	= -i
include ../../CONFIG

clean:
	rm -f $(ALL)

install: $(ALL)
	cp $(ALL) $(GAMDIR)
	cd $(GAMDIR); chmod 711 $(ALL)
	cp headline $(GAMLIB)
	cd $(GAMLIB); chmod 644 headline
