#
#	Dam (draghts) $Header: /home/Vince/cvs/games.d/draughts.d/Makefile,v 1.5 2002-11-23 04:09:38 Vincent Exp $
#
ALL	= dam$(EXE)
MODEL	= -i -F 2000
OPTIONS	= -DHELPFIL=\"$(GAMLIB)/dam.help\"
include $(HOME)/CONFIG
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
