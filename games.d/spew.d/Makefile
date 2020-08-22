#
#	$Header: /home/Vince/cvs/games.d/spew.d/Makefile,v 1.4 2002-11-23 04:09:47 Vincent Exp $
#
ALL	= spew$(EXE)
OPTIONS = -O -DDEFFILE=\"$(GAMLIB)/headline\"
MODEL	= -i
include $(HOME)/CONFIG

clean:
	rm -f $(ALL)

install: $(ALL)
	cp $(ALL) $(GAMDIR)
	cd $(GAMDIR); chmod 711 $(ALL)
	cp headline $(GAMLIB)
	cd $(GAMLIB); chmod 644 headline
