#
#	$Header: /home/Vince/cvs/games.d/Makefile,v 1.2 1987-06-08 22:02:38 vrs Exp $
#
TODO	= phantasia rain reversi robomatic robots rogue sail smut \
	  snake sniglets solve spaceduel tko trek73 vtrek worms
ALL	= adv adventure advgen aliens atc battleship battlestar blackbox \
	  cbj corewar craps draughts fishomatic flibs greed hack hack1.0.3 \
	  hangman hangman2 hanoi larn11 larn12 life mazewar midway mille \
	  noughts ogre pdrogue phantasia plane pm poker rain reversi \
	  robomatic robots rogue rot sail smut snake sniglets solve spaceduel \
	  spew tko trek73 valspeak vtrek wator worms zork
include ../CONFIG

install:
	@for i in $(ALL); do cd $$i.d; make install; cd ..; done

clean:
	@for i in $(ALL); do cd $$i.d; make clean; cd ..; done

hangman2: $(LIBRS)
mazewar: $(LIBRS)
poker: $(LIBRS)
