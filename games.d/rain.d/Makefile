#
#	Rain $Header: /home/Vince/cvs/games.d/rain.d/Makefile,v 1.3 1990-03-27 17:27:02 vrs Exp $
#
ALL	= rain
MODEL	= -i -F 2000
OPTIONS	= -O
include /etc/CONFIG
CFILES	= rain.c

lint: $(CFILES) 
	lint -h $(CFILES) $(LIBS) 

install: $(ALL) $(GAMDIR)
	cp $(ALL) $(GAMDIR)
	cd $(GAMDIR); chmod 711 $(ALL)

clean:
	rm -f $(ALL) make.out core
