#
#	$Header: /home/Vince/cvs/games.d/aliens.d/Makefile,v 1.4 1990-03-27 17:24:27 vrs Exp $
#	Aliens -- A satire of human conflict with the invaders
#
ALL	= aliens
OPTIONS	= -O
MODEL	= -i -F 2000
include /etc/CONFIG

install: $(ALL) $(GAMDIR)
	cp $(ALL) $(GAMDIR)
	cd $(GAMDIR); chmod 711 $(ALL)

clean:
	rm -f $(ALL) core make.out
