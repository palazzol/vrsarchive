#
#	zork $Header: /home/Vince/cvs/games.d/zork.d/Makefile,v 1.3 1987-12-26 21:38:24 vrs Exp $
#
ALL = zork dindx.dat dtext.dat dng
include ../../CONFIG
LIBZORK	= DUNGEON.DOC dindx.dat dtext.dat dng

install: $(ALL) $(GAMDIR) $(GAMLIB)
	cp $(ALL) $(GAMDIR)
	cd $(GAMDIR); chmod 755 $(ALL)
	cp $(LIBZORK) $(GAMLIB)
	cd $(GAMLIB); chmod 644 $(LIBZORK)
	cd $(GAMLIB); chmod 711 $(GAMLIB)/dng

dindx.dat: dindx.dat.uu
	uudecode $?

dtext.dat: dtext.dat.uu
	uudecode $?

dng:	dng.uu
	uudecode $?

zork:	zork.sh ../../CONFIG
	sed '/^DNGLIB/c\
DNGLIB=$(GAMLIB)' zork.sh > $@

clean:
	rm -f $(ALL)
