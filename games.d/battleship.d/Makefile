#
#	%W% %G%
#
ALL	= battleship$(EXE)
MODEL	= -i -F 2000
include $(HOME)/CONFIG

install:	$(ALL) $(GAMDIR)
		cp $(ALL) $(GAMDIR)
		cd $(GAMDIR); chmod 711 $(ALL)

lint:		battleship.c
		lint -lcurses -ltermcap battleship.c | tee $@

clean:
		rm -f $(ALL)
