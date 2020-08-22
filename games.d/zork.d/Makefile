#
#	zork $Header: /home/Vince/cvs/games.d/zork.d/Makefile,v 1.7 2002-11-24 06:45:52 Vincent Exp $
#
ALL = zork dindx.dat dtext.dat dng
include $(HOME)/CONFIG
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

zork:	zork.sh $(HOME)/CONFIG
	sed '/^DNGLIB/c\
DNGLIB=$(GAMLIB)' zork.sh > $@

clean:
	rm -f $(ALL) dng dtext.dat dindx.dat
