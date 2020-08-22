#
#	Blackjack $Header: /home/Vince/cvs/games.d/cbj.d/Makefile,v 1.4 2002-11-23 04:09:36 Vincent Exp $
#
ALL	= cbj$(EXE)
MODEL	= -i
OPTIONS	= -O
include $(HOME)/CONFIG

install: $(ALL) $(GAMDIR) $(MAN6)
	cp $(ALL) $(GAMDIR)
	cd $(GAMDIR); chmod 711 $(ALL)
	cp cbj.6 $(MAN6)
	chmod 644 $(MAN6)/cbj.6

clean:
	rm -f $(ALL)
