#
#	life $Header: /home/Vince/cvs/games.d/life.d/Makefile,v 1.4 2002-11-23 04:09:40 Vincent Exp $
#
OPTIONS	= -Dreg=register
ALL	= life$(EXE)
MODEL	= -F 2000
include $(HOME)/CONFIG

install: $(ALL) $(GAMDIR)
	cp $(ALL) $(GAMDIR)
	cd $(GAMDIR); chmod 711 $(ALL)

clean:
	rm -f $(ALL)
