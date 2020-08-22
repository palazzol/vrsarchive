#
#	$Header: /home/Vince/cvs/games.d/Makefile,v 1.13 2002-11-26 07:35:47 vrs Exp $
#
# CYGWIN: Othello is in modula-2, need a compiler.
# CYGWIN: Suzy requires a lisp implementation.
# CYGWIN: Zork is a binary in an incompatible format.
ALL	= adl adv adventure advgen aliens atc banner battleship battlestar \
	  blackbox cbj centipede chess conquer conquest corewar craps draughts \
	  fishomatic flibs fortune gothic hack1.0.3 hangman hangman2 \
	  hanoi larn12 life mazewar midway mille ncode nethack nethack3.0 \
	  noughts ogre omega pdrogue phantasia plane pm poker rain reversi \
	  rgm robomatic robots rogue rot sail smut snake sniglets solve \
	  spaceduel spacewar spew tetrix tko trek73 tttt umoria \
	  valspeak vtrek warp wator world worms # dungeon othello suzy zork
include $(HOME)/CONFIG

install:
	@for i in $(ALL); do (cd $$i.d && make install); done

clean:
	@for i in $(ALL); do (cd $$i.d && make clean); done

hangman2: $(LIBRS)
mazewar: $(LIBRS)
poker: $(LIBRS)
