#
#	snake %W% %G%
#
#
GAMEDIR	= /usr/games
SHELL	= /bin/sh
LFLAGS	= -F 2000	# XENIX only
LIBS	= -lcurses -ltermlib

all:	snake snake2

snake:	snake.o
	cc $(LFLAGS) -o snake snake.o $(LIBS)

snake2: snake2.o
	cc $(LFLAGS) -o snake2 snake2.o $(LIBS)

lint:
	lint -lcurses -ltermlib snake.c | tee lint.out

clean:
	rm -f core *.o snake snake2 a.out

$(GAMEDIR):
	mkdir $@
	chmod 755 $@

install: snake snake2 $(GAMEDIR)
	cp snake $(GAMEDIR)
	chmod 711 $(GAMEDIR)/snake
	cp snake2 $(GAMEDIR)
	chmod 711 $(GAMEDIR)/snake2
