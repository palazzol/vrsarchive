#
#	Dam (draghts) @(#)Makefile	1.2 8/4/85
#
SHELL	= /bin/sh
GAMEDIR	= $(DESTDIR)/usr/games
GAMELIB	= $(GAMEDIR)/lib
CFLAGS	= -DHELPFIL=\"$(GAMELIB)/dam.help\"
OBJECT	= dam.o prbord.o prinfo.o readmove.o timeac.o
LFLAGS	= -F 2000 -lcurses -ltermlib

dam:	$(OBJECT)
	cc -o dam $(OBJECT) $(LFLAGS)

lint:
	lint -lcurses -ltermcap $(OBJECT:.o=.c)

$(OBJECT): damdefs.h

clean:
	rm -f $(OBJECT) lint.out make.out core dam

$(GAMEDIR) $(GAMELIB):
	mkdir $@
	chmod 755 $@

install: $(GAMEDIR) $(GAMELIB) dam
	cp dam $(GAMEDIR)
	chmod 711 $(GAMEDIR)/dam
	cp dam.help $(GAMELIB)
	chmod 644 $(GAMELIB)/dam.help
