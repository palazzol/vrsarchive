#
#	$Header: /home/Vince/cvs/games.d/spaceduel.d/Makefile,v 1.5 2002-11-23 04:09:47 Vincent Exp $
#
ALL	= spaceduel$(EXE)
OPTIONS	= -O -DGAMLIB=$(GAMLIB) -DPAGER=\"$(PAGER)\" #-DWIZARD
MODEL	= -i 
include $(HOME)/CONFIG

install: $(ALL) $(GAMDIR)
	cp $(ALL) $(GAMDIR)
	cd $(GAMDIR); chown $(SAFEUID) $(ALL)
	cd $(GAMDIR); chmod 4711 $(ALL)
	cd $(GAMLIB); touch sd.logfile
	cd $(GAMLIB); chown $(SAFEUID) sd.logfile
	cd $(GAMLIB); chmod 644 sd.logfile

clean:
	rm -f $(ALL) core make.out

lint:
	lint $(CFLAGS) $(LIBS) spaceduel.c
