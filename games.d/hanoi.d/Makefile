#
#	hanoi $Header: /home/Vince/cvs/games.d/hanoi.d/Makefile,v 1.3 1990-03-27 17:25:51 vrs Exp $
#
#
ALL	= hanoi
MODEL	= -i -F 2000	# XENIX only
OPTIONS	= -O
include /etc/CONFIG

lint:
	lint -lcurses -ltermlib hanoi.c

clean:
	rm -f $(ALL)

install: $(ALL) $(GAMDIR)
	cp $(ALL) $(GAMDIR)
	cd $(GAMDIR); chmod 711 $(ALL)
