#
#	$Header: /home/Vince/cvs/games.d/atc.d/Makefile,v 1.8 2002-11-24 18:42:40 vrs Exp $
#
ALL	= makesrc distrib
include $(HOME)/CONFIG
GAME	= atc$(EXE)
SRC	= src
DIST	= dist
GAMES	= games
TUNABLE	= tunable.c
NF	= $(GAME) -p
MAKEFILE= Makefile.dist
CURSESLIBR= libatc.a

install:	$(ALL) $(DIST)
		(cd $(DIST); make)
		-mkdir `$(DIST)/$(NF)`
		-cp $(GAMES)/* `$(DIST)/$(NF)`
		-chmod 644 `$(DIST)/$(NF)`/*
		-chmod 755 `$(DIST)/$(NF)`
		-chown $(SAFEUID) `$(DIST)/$(NF)`/* `$(DIST)/$(NF)`
		cd $(DIST); cp $(GAME) $(GAMDIR)
		cd $(GAMDIR); chown $(SAFEUID) $(GAME)
		cd $(GAMDIR); chmod 4755 $(GAME)

distrib:	$(DIST) $(DIST)/Makefile $(DIST)/$(TUNABLE) \
		$(DIST)/$(CURSESLIBR)

makesrc:	
		@(cd $(SRC); make RANLIB=$(RANLIB) )

$(DIST):
		mkdir $(DIST)

$(DIST)/$(TUNABLE):	$(SRC)/$(TUNABLE)
		rm -f $(DIST)/$(TUNABLE)
		cp $(SRC)/$(TUNABLE) $(DIST)/$(TUNABLE)
		chmod 644 $(DIST)/$(TUNABLE)
		
$(DIST)/Makefile:	$(SRC)/$(MAKEFILE)
		rm -f $(DIST)/Makefile
		cp $(SRC)/$(MAKEFILE) $(DIST)/Makefile
		
$(DIST)/$(CURSESLIBR):	$(SRC)/$(CURSESLIBR)
		rm -f $(DIST)/$(CURSESLIBR)
		cp $(SRC)/$(CURSESLIBR) $(DIST)/$(CURSESLIBR)
		$(RANLIB) $(DIST)/$(CURSESLIBR)
		
archive:
		shar Makefile README games/* src/*.[chly] doc/* > ARCH

clean:
	cd src; make clean
	rm -rf dist
