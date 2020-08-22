#
#	corewar $Header: /home/Vince/cvs/games.d/corewar.d/Makefile,v 1.4 2002-11-23 04:09:37 Vincent Exp $
#
ALL	= corewar$(EXE)
MODEL	= -i
OPTIONS	= -O
include $(HOME)/CONFIG
CORLIB	= $(GAMLIB)/corewar
XMPLES	= backz bigfoot duh dwarf dwarf1 dwarf2 fork_imp \
	  gemini gemini2 imp juggernaut

clean:
	rm -f $(ALL)

$(CORLIB):
	mkdir $@
	chmod 755 $@

install: $(ALL) $(GAMDIR) $(GAMLIB) $(CORLIB) $(MAN6)
	cp $(ALL) $(GAMDIR)
	cd $(GAMDIR); chmod 711 $(ALL)
	cp corewar.6 $(MAN6)
	chmod 644 $(MAN6)/corewar.6
	cp $(XMPLES) $(CORLIB)
	cd $(CORLIB); chmod 644 $(XMPLES)
