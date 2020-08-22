#
#	$Header: /home/Vince/cvs/games.d/ncode.d/Makefile,v 1.3 2002-11-23 04:09:41 Vincent Exp $
#
ALL	= ncode$(EXE) headline nonsense
OPTIONS = -O
MODEL	= -i
include $(HOME)/CONFIG
NCOLIB	= $(GAMLIB)/ncode

clean:
	rm -f $(ALL)

$(NCOLIB):
	mkdir $@
	chmod 755 $@

install: $(ALL) $(GAMDIR) $(GAMLIB) $(NCOLIB)
	cp ncode$(EXE) $(NCOLIB)/
	cd $(NCOLIB); chmod 711 ncode$(EXE)
	cp head.nc nonsense.nc $(NCOLIB)/
	cd $(NCOLIB); chmod 644 head.nc nonsense.nc
	cp headline nonsense $(GAMDIR)/
	cd $(GAMDIR); chmod 755 headline nonsense
	cp ncode.1 $(MAN6)/
	cd $(MAN6); chmod 644 ncode.1
