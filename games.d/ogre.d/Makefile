#
#	Ogre %W% %G%
#
GAMEDIR	= /usr/games
USRMAN	= /usr/man
MANDIR	= $(USRMAN)/man6
SHELL	= /bin/sh
OBJ	= init.o termcap.o map.o main.o move.o initround.o ogrecom.o \
	  ogrestat.o attack.o resolve.o

ogre: $(OBJ)
	cc -F 2000 -o ogre $(OBJ) -ltermcap

$(GAMEDIR) $(USRMAN) $(MANDIR):
	mkdir $@
	chmod 755 $@

install: ogre $(GAMEDIR) $(USRMAN) $(MANDIR)
	cp ogre $(GAMEDIR)
	chmod 711 $(GAMEDIR)/ogre
	cp ogre.6 $(MANDIR)
	chmod 644 $(MANDIR)/ogre.6

clean:
	rm -f $(OBJ) ogre lint.out tags

lint:
	lint init.c termcap.c map.c main.c move.c initround.c ogrecom.c  \
		ogrestat.c attack.c resolve.c | tee lint.out

tags:	init.c termcap.c map.c main.c move.c initround.c ogrecom.c  \
		ogrestat.c attack.c resolve.c ogre.h ext.h
	rm -f tags
	ctags init.c termcap.c map.c main.c move.c initround.c ogrecom.c  \
		ogrestat.c attack.c resolve.c ogre.h ext.h

init.o: init.c ogre.h
main.o: main.c ogre.h
move.o: move.c ogre.h
attack.o: attack.c ogre.h
resolve.o: resolve.c ogre.h
initround.o: initround.c ogre.h
termcap.o: termcap.c
ogrecom.o: ogrecom.c ogre.h
ogrestat.o: ogrestat.c ogre.h
map.o: map.c ogre.h
backup:
	tar cvf ogre.tar *
