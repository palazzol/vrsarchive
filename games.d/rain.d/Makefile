#
#	Rain $Header: /home/Vince/cvs/games.d/rain.d/Makefile,v 1.4 2002-11-23 04:09:44 Vincent Exp $
#
ALL	= rain$(EXE)
MODEL	= -i -F 2000
OPTIONS	= -O
include $(HOME)/CONFIG
CFILES	= rain.c

lint: $(CFILES) 
	lint -h $(CFILES) $(LIBS) 

install: $(ALL) $(GAMDIR)
	cp $(ALL) $(GAMDIR)
	cd $(GAMDIR); chmod 711 $(ALL)

clean:
	rm -f $(ALL) make.out core
