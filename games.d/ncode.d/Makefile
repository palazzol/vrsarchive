#
#	$Header: /home/Vince/cvs/games.d/ncode.d/Makefile,v 1.2 1990-03-27 17:26:24 vrs Exp $
#
ALL	= ncode headline nonsense
OPTIONS = -O
MODEL	= -i
include /etc/CONFIG
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
