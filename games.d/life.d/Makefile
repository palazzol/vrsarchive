#
#	life $Header: /home/Vince/cvs/games.d/life.d/Makefile,v 1.3 1990-03-27 17:26:06 vrs Exp $
#
ALL	= life
MODEL	= -F 2000
include /etc/CONFIG

install: $(ALL) $(GAMDIR)
	cp $(ALL) $(GAMDIR)
	cd $(GAMDIR); chmod 711 $(ALL)

clean:
	rm -f $(ALL)
