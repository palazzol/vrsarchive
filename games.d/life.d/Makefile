#
#	life $Header: /home/Vince/cvs/games.d/life.d/Makefile,v 1.2 1987-06-08 21:26:29 vrs Exp $
#
ALL	= life
MODEL	= -F 2000
include ../../CONFIG

install: $(ALL) $(GAMDIR)
	cp $(ALL) $(GAMDIR)
	cd $(GAMDIR); chmod 711 $(ALL)

clean:
	rm -f $(ALL)
