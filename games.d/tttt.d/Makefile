#
#	tttt $Header: /home/Vince/cvs/games.d/tttt.d/Makefile,v 1.5 1990-05-21 14:28:42 vrs Exp $
#
ALL	= tttt
MODEL	= -i -F 2000
OPTIONS	= -O
include /etc/CONFIG
CFILES	= rain.c

lint: $(CFILES) 
	lint -h $(CFILES) $(LIBS) 

install: $(ALL) $(GAMDIR)
	cp $(ALL) $(GAMDIR)
	cd $(GAMDIR); chmod 711 $(ALL)
	cp tttt.6 $(MAN6)
	cd $(MAN6); chmod 644 tttt.6

clean:
	rm -f $(ALL) make.out core
