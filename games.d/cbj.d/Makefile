#
#	Blackjack $Header: /home/Vince/cvs/games.d/cbj.d/Makefile,v 1.2 1987-06-08 21:24:56 vrs Exp $
#
ALL	= cbj
MODEL	= -i
OPTIONS	= -O
include ../../CONFIG

install: $(ALL) $(GAMDIR) $(MAN6)
	cp $(ALL) $(GAMDIR)
	cd $(GAMDIR); chmod 711 $(ALL)
	cp cbj.6 $(MAN6)
	chmod 644 $(MAN6)/cbj.6

clean:
	rm -f $(ALL)
