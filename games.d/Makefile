#
#	$Header: /home/Vince/cvs/games.d/Makefile,v 1.10 1990-04-12 13:20:21 vrs Exp $
#
TODO = conquer
ALL	= adl adv adventure advgen aliens atc battleship battlestar blackbox \
	  cbj centipede chess conquest corewar craps crystal draughts \
	  fishomatic flibs fortune greed hack1.0.3 hangman hangman2 hanoi larn11 \
	  larn12 life mazewar midway mille ncode nethack nethack3.0 noughts ogre \
	  omega pdrogue phantasia plane pm poker rain reversi rgm robomatic \
	  robots rogue rot sail search search2 smut snake sniglets solve \
	  spaceduel spew tko trek73 valspeak umoria valspeak vtrek warp wator \
	  world worms zork
include /etc/CONFIG

install:
	@for i in $(ALL); do cd $$i.d; make install; cd ..; done

clean:
	@for i in $(ALL); do cd $$i.d; make clean; cd ..; done

hangman2: $(LIBRS)
mazewar: $(LIBRS)
poker: $(LIBRS)
