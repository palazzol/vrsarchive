#
#	zork $Header: /home/Vince/cvs/games.d/zork.d/Makefile,v 1.5 1990-04-04 21:31:48 vrs Exp $
#
ALL = zork dindx.dat dtext.dat dng
include /etc/CONFIG
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

zork:	zork.sh /etc/CONFIG
	sed '/^DNGLIB/c\
DNGLIB=$(GAMLIB)' zork.sh > $@

clean:
	rm -f $(ALL)
