#
#	Aliens -- A satire of human conflict with the invaders
#
GAMES	= /usr/games
SHELL	= /bin/sh

aliens:	aliens.o
	cc aliens.o -o $@

$(GAMES):
	mkdir $@
	chmod 755 $@

install: aliens $(GAMES)
	cp aliens $(GAMES)
	chmod 711 $(GAMES)/aliens

clean:
	rm -f aliens *.o core make.out
