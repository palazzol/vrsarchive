#
#	$Header: /home/Vince/cvs/games.d/Makefile,v 1.8 1990-03-27 17:23:57 vrs Exp $
#
TODO	= phantasia rain reversi robomatic robots rogue sail smut \
	  snake sniglets solve spaceduel tko trek73 vtrek worms
SYS5	= search
XENIX	= greed mazewar phantasia poker zork
ALL	= adv adventure advgen aliens atc battleship battlestar blackbox \
	  cbj chess corewar craps draughts fishomatic flibs fortune \
	  hack1.0.3 hangman hangman2 hanoi larn11 larn12 life midway mille \
	  nethack ncode noughts ogre pdrogue plane pm rain reversi rgm \
	  robomatic robots rogue rot sail smut snake sniglets solve spaceduel \
	  spew tko trek73 valspeak vtrek warp wator worms
include /etc/CONFIG

install:
	@for i in $(ALL); do cd $$i.d; make install; cd ..; done

clean:
	@for i in $(ALL); do cd $$i.d; make clean; cd ..; done

hangman2: $(LIBRS)
mazewar: $(LIBRS)
poker: $(LIBRS)
