#
#	Ogre $Header: /home/Vince/cvs/games.d/ogre.d/Makefile,v 1.5 2002-11-24 02:13:42 Vincent Exp $
#
ALL	= ogre$(EXE)
MODEL	= -i -F 2000
OPTIONS	= -O
include $(HOME)/CONFIG
OBJ	= init.o termcap.o map.o main.o move.o initround.o ogrecom.o \
	  ogrestat.o attack.o resolve.o

ogre$(EXE): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $(OBJ) $(LIBS)

install: $(ALL) $(GAMDIR) $(MAN6)
	cp $(ALL) $(GAMDIR)
	cd $(GAMDIR); chmod 711 $(ALL)
	cp ogre.6 $(MAN6)
	chmod 644 $(MAN6)/ogre.6

clean:
	rm -f $(ALL) $(OBJ) lint.out tags

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
