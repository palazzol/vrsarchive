#
#	zork $Header: /home/Vince/cvs/games.d/zork.d/Makefile,v 1.2 1987-06-08 22:03:50 vrs Exp $
#
ALL = zork
include ../../CONFIG
LIBZORK	= DUNGEON.DOC dindx.dat dtext.dat dng

install: $(ALL) $(GAMDIR) $(GAMLIB)
	cp $(ALL) $(GAMDIR)
	cd $(GAMDIR); chmod 755 $(ALL)
	cp $(LIBZORK) $(GAMLIB)
	cd $(GAMLIB); chmod 644 $(LIBZORK)
	cd $(GAMLIB); chmod 711 $(GAMLIB)/dng

zork:	zork.sh ../../CONFIG
	sed '/^DNGLIB/c\
DNGLIB=$(GAMLIB)' zork.sh > $@

clean:
	rm -f $(ALL)
