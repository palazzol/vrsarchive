SHELL	= /bin/sh
LIBRS	= berkeley.t dpy.t
TARGETS	= adv.t adventure.t advgen.t aliens.t battleship.t battlestar.t \
	  blackbox.t cbj.t corewar.t craps.t draughts.t fishomatic.t \
	  flibs.t hack.t hack1.0.3.t hangman.t hangman2.t hanoi.t life.t \
	  larn11.t larn12.t mazewar.t midway.t mille.t noughts.t ogre.t \
	  pdrogue.t phantasia.t plane.t pm.t poker.t rain.t reversi.t \
	  robomatic.t robots.t rogue.t sail.t smut.t snake.t sniglets.t \
	  spaceduel.t solve.t tko.t trek73.t vtrek.t worms.t zork.t

.SUFFIXES: .d .t

.d.t:
	cd $<; make install clean
	touch $@

install: $(TARGETS)

hangman2.t: $(LIBRS)
mazewar.t: $(LIBRS)
poker.t: $(LIBRS)

clean:
	for i in $(TARGETS:.t=.d); do cd $$i; make clean; cd ..; done
	rm -f $(TARGETS) $(LIBRS) berkeley.d/*berkeley.a dpy.d/*dpy.a
