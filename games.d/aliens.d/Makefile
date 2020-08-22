#
#	$Header: /home/Vince/cvs/games.d/aliens.d/Makefile,v 1.5 2002-11-23 04:09:35 Vincent Exp $
#	Aliens -- A satire of human conflict with the invaders
#
ALL	= aliens$(EXE)
OPTIONS	= -O
MODEL	= -i -F 2000
include $(HOME)/CONFIG

install: $(ALL) $(GAMDIR)
	cp $(ALL) $(GAMDIR)
	cd $(GAMDIR); chmod 711 $(ALL)

clean:
	rm -f $(ALL) core make.out
