#
#	hanoi $Header: /home/Vince/cvs/games.d/hanoi.d/Makefile,v 1.4 2002-11-23 04:09:40 Vincent Exp $
#
#
ALL	= hanoi$(EXE)
MODEL	= -i -F 2000	# XENIX only
OPTIONS	= -O
include $(HOME)/CONFIG

lint:
	lint -lcurses -ltermlib hanoi.c

clean:
	rm -f $(ALL)

install: $(ALL) $(GAMDIR)
	cp $(ALL) $(GAMDIR)
	cd $(GAMDIR); chmod 711 $(ALL)
