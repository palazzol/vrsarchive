#
#	%W% %G%
#
SHELL	= /bin/sh
CFLAGS	= -i
LFLAGS	= -F 2000 -lcurses -ltermcap
GAMDIR	= $(DESTDIR)/usr/games

battleship:	battleship.o
		cc $(CFLAGS) -o $@ battleship.o $(LFLAGS)

$(GAMDIR):
		mkdir $@
		chmod 755 $@

install:	battleship $(GAMDIR)
		cp battleship $(GAMDIR)
		chmod 711 $(GAMDIR)/battleship

lint:		battleship.c
		lint -lcurses -ltermcap battleship.c | tee $@

clean:
		rm -f battleship battleship.o
