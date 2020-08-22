#
# Makefile for chess program
#
ALL	= chess$(EXE)
OPTIONS	= -O -DGAMLIB=$(GAMLIB)
MODEL	= -i
include $(HOME)/CONFIG

clean:
	rm -f $(ALL)

install: $(ALL) $(GAMDIR) $(GAMLIB)
	cp $(ALL) $(GAMDIR)
	cd $(GAMDIR); chmod 711 $(ALL)
	cp chess.opn $(GAMLIB)
	cd $(GAMLIB); chmod 644 chess.opn
