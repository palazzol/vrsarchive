#
#	$Header: /home/Vince/cvs/games.d/aliens.d/Makefile,v 1.2 1987-06-08 21:24:01 vrs Exp $
#	Aliens -- A satire of human conflict with the invaders
#
ALL	= aliens
include ../../CONFIG

install: $(ALL) $(GAMDIR)
	cp $(ALL) $(GAMDIR)
	cd $(GAMDIR); chmod 711 $(ALL)

clean:
	rm -f $(ALL) core make.out
