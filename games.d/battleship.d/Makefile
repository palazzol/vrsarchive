#
#	%W% %G%
#
ALL	= battleship
MODEL	= -i -F 2000
include ../../CONFIG

install:	$(ALL) $(GAMDIR)
		cp $(ALL) $(GAMDIR)
		cd $(GAMDIR); chmod 711 $(ALL)

lint:		battleship.c
		lint -lcurses -ltermcap battleship.c | tee $@

clean:
		rm -f $(ALL)
