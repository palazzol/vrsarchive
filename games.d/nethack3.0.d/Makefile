#	Hack Makefile.
#	SCCS Id: @(#)Makefile.top	3.0	89/03/23

# make NetHack
GAME	= nethack3.0
ALL		= $(GAME) data rumors spec_levs
include /etc/CONFIG
GAMEUID	= $(SAFEUID)
GAMEGRP	= $(SAFEGRP)

#	GAMEDIR also appears in config.h as "HACKDIR".
GAMEDIR  = $(GAMLIB)/nethack3.0
SHELLDIR = $(GAMEDIR)
MANEXT   = 6
# manual installation for most BSD-style systems
GAMEMANCREATE = cp auxil/nhack.6
LEVMANCREATE = cp auxil/lev_comp.6
# manual installation for most SYSV-style systems
# GAMEMANCREATE = $(NROFF) -man auxil/nhack.6 >
# LEVMANCREATE = $(NROFF) -man auxil/lev_comp.6 >

AUXHELP = help hh cmdhelp history opthelp oracles
VARAUX = data rumors
SPEC_LEVS = endgame castle tower1 tower2 tower3
AUX = $(AUXHELP) $(VARAUX) $(SPEC_LEVS) license

$(GAME):
	( cd src ; make )

all:	$(GAME) Guidebook data rumors spec_levs
	@echo "Done."

Guidebook:
	( cd auxil ; make Guidebook )

data:
	( cd auxil ; make data )

rumors:
	( cd auxil ; make rumors )

spec_levs:
	( cd src ; make lev_comp )
	( cd auxil ; make spec_levs )
	
$(SHELLDIR) $(GAMEDIR) $(GAMEDIR)/save:
	mkdir $@
	chmod 755 $@

install update: $(ALL) $(SHELLDIR) $(GAMEDIR) $(GAMEDIR)/save $(MAN6)
	chown $(GAMEUID) $(GAMEDIR)
	chgrp $(GAMEGRP) $(GAMEDIR)
	touch $(GAMEDIR)/perm $(GAMEDIR)/record $(GAMEDIR)/logfile
	chown $(GAMEUID) $(GAMEDIR)/perm $(GAMEDIR)/record $(GAMEDIR)/logfile
	chgrp $(GAMEGRP) $(GAMEDIR)/perm $(GAMEDIR)/record $(GAMEDIR)/logfile
	chmod 644 $(GAMEDIR)/perm $(GAMEDIR)/record $(GAMEDIR)/logfile
	( cd auxil ; cp $(AUX) $(GAMEDIR) )
	( cd $(GAMEDIR) ; chown $(GAMEUID) $(AUX) )
	( cd $(GAMEDIR) ; chgrp $(GAMEGRP) $(AUX) )
	( cd $(GAMEDIR) ; chmod 644 $(AUX) )
	sed "s:GAMEDIR:$(GAMEDIR):" auxil/nethack.sh >$(SHELLDIR)/$(GAME)
	chown $(GAMEUID) $(SHELLDIR)/$(GAME) 
	chgrp $(GAMEGRP) $(SHELLDIR)/$(GAME) 
	chmod 755 $(SHELLDIR)/$(GAME)
	cp src/$(GAME) $(GAMDIR)/$(GAME)
	chown $(GAMEUID) $(GAMDIR)/$(GAME) 
	chgrp $(GAMEGRP) $(GAMDIR)/$(GAME) 
	chmod 04755 $(GAMDIR)/$(GAME)
	$(GAMEMANCREATE) $(MAN6)/$(GAME).$(MANEXT)
	$(LEVMANCREATE) $(MAN6)/lev_comp.$(MANEXT)

clean:
	( cd auxil ; make clean)
	( cd src ; make clean)

spotless:
	( cd auxil ; make clean)
	( cd src ; make spotless)
