#
# Makefile for atc parent
#

OWNER =		cron
DESTINATION =	/usr/games

GAME =		atc
SRC =		src
DIST =		dist
GAMES =		games

CURSESLIBR =	libatc.a
TUNABLE	=	tunable.c
NF      =	$(GAME) -p
MAKEFILE =	Makefile.dist

all:		makesrc install

install:	distrib $(DIST)
		(cd $(DIST); make)
		-mkdir `$(DIST)/$(NF)`
		-cp $(GAMES)/* `$(DIST)/$(NF)`
		-chmod 644 `$(DIST)/$(NF)`/*
		-chmod 755 `$(DIST)/$(NF)`
		-chown $(OWNER) `$(DIST)/$(NF)`/* `$(DIST)/$(NF)`
		cd $(DIST); cp $(GAME) $(DESTINATION)
		cd $(DESTINATION); chown $(OWNER) $(GAME)
		cd $(DESTINATION); chmod 4755 $(GAME)

distrib:	$(DIST)/Makefile $(DIST)/$(TUNABLE) \
		$(DIST)/$(CURSESLIBR)

makesrc:	
		( cd $(SRC); make $(CURSESLIBR) )

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
		
archive:
		shar Makefile README games/* src/*.[chly] doc/* > ARCH
