#
#	$Header: /home/Vince/cvs/games.d/ncode.d/Makefile,v 1.1 1987-06-28 19:32:21 vrs Exp $
#
ALL	= ncode headline nonsense
OPTIONS = -O
MODEL	= -i
include ../../CONFIG
NCOLIB	= $(GAMLIB)/ncode

clean:
	rm -f $(ALL)

$(NCOLIB):
	mkdir $@
	chmod 755 $@

install: $(ALL) $(GAMDIR) $(GAMLIB) $(NCOLIB)
	cp ncode $(NCOLIB)/
	cd $(NCOLIB); chmod 711 ncode
	cp head.nc nonsense.nc $(NCOLIB)/
	cd $(NCOLIB); chmod 644 head.nc nonsense.nc
	cp headline nonsense $(GAMDIR)/
	cd $(GAMDIR); chmod 755 headline nonsense
	cp ncode.1 $(MAN6)/
	cd $(MAN6); chmod 644 ncode.1
