#
#	$Header: /home/Vince/cvs/games.d/aliens.d/Makefile,v 1.3 1987-06-25 18:33:14 vrs Exp $
#	Aliens -- A satire of human conflict with the invaders
#
ALL	= aliens
OPTIONS	= -O
MODEL	= -i -F 2000
include ../../CONFIG

install: $(ALL) $(GAMDIR)
	cp $(ALL) $(GAMDIR)
	cd $(GAMDIR); chmod 711 $(ALL)

clean:
	rm -f $(ALL) core make.out
