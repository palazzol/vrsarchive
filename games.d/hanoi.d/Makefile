#
#	hanoi $Header: /home/Vince/cvs/games.d/hanoi.d/Makefile,v 1.2 1987-06-08 21:26:18 vrs Exp $
#
#
ALL	= hanoi
MODEL	= -i -F 2000	# XENIX only
OPTIONS	= -O
include ../../CONFIG

lint:
	lint -lcurses -ltermlib hanoi.c

clean:
	rm -f $(ALL)

install: $(ALL) $(GAMDIR)
	cp $(ALL) $(GAMDIR)
	cd $(GAMDIR); chmod 711 $(ALL)
