#
#	zork %W %G%
#
#
GAMDIR	= $(DSTDIR)/usr/games
GAMLIB	= $(GAMDIR)/lib
LIBZORK	= DUNGEON.DOC dindx.dat dtext.dat dng
GAMZORK = zork

install: $(GAMDIR) $(GAMLIB)
	cp $(GAMZORK) $(GAMDIR)
	cd $(GAMDIR); chmod 711 $(GAMZORK)
	cp $(LIBZORK) $(GAMLIB)
	cd $(GAMLIB); chmod 644 $(LIBZORK)
	cd $(GAMLIB); chmod 711 $(GAMLIB)/dng

$(GAMDIR) $(GAMLIB):
	mkdir $@
	chmod 755 $@

clean:
