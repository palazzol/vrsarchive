#
#	Dam (draghts) $Header: /home/Vince/cvs/games.d/draughts.d/Makefile,v 1.3 1987-06-08 21:28:15 vrs Exp $
#
ALL	= dam
MODEL	= -i -F 2000
OPTIONS	= -DHELPFIL=\"$(GAMLIB)/dam.help\"
include ../../CONFIG
OBJECT	= dam.o prbord.o prinfo.o readmove.o timeac.o

dam:	$(OBJECT)
	$(CC) $(OBJECT) -o $@ $(LIBS)

lint:
	lint -lcurses -ltermcap $(OBJECT:.o=.c)

$(OBJECT): damdefs.h

clean:
	rm -f $(OBJECT) $(ALL)

install: $(ALL) $(GAMDIR) $(GAMLIB)
	cp $(ALL) $(GAMDIR)
	cd $(GAMDIR); chmod 711 $(ALL)
	cp dam.help $(GAMLIB)
	chmod 644 $(GAMLIB)/dam.help
