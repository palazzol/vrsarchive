#
#	Rain $Header: /home/Vince/cvs/games.d/rain.d/Makefile,v 1.2 1987-06-08 21:28:05 vrs Exp $
#
ALL	= rain
MODEL	= -i -F 2000
OPTIONS	= -O
include ../../CONFIG
CFILES	= rain.c

lint: $(CFILES) 
	lint -h $(CFILES) $(LIBS) 

install: $(ALL) $(GAMDIR)
	cp $(ALL) $(GAMDIR)
	cd $(GAMDIR); chmod 711 $(ALL)

clean:
	rm -f $(ALL) make.out core
