#
#	$Header: /home/Vince/cvs/games.d/spaceduel.d/Makefile,v 1.2 1987-06-14 18:54:21 vrs Exp $
#
ALL	= spaceduel
OPTIONS	= -O -DGAMLIB=\"$(GAMLIB) -DPAGER=\"$(PAGER)\" #-DWIZARD
MODEL	= -i 
include ../../CONFIG

install: $(ALL) $(GAMDIR)
	cp $(ALL) $(GAMDIR)/spaceduel
	cd $(GAMDIR); chown $(SAFEUID) $(ALL)
	cd $(GAMDIR); chmod 4711 $(ALL)
	cd $(GAMLIB); touch sd.logfile
	cd $(GAMLIB); chown $(SAFEUID) sd.logfile
	cd $(GAMLIB); chmod 644 sd.logfile

clean:
	rm -f $(ALL) core make.out

lint:
	lint $(CFLAGS) $(LIBS) spaceduel.c
