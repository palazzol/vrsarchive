# Simple Makefile for spew

HEADLNS = \"/usr/games/headline\"

CFLAGS = -O -DDEFFILE=$(HEADLNS)

spew:	spew.c
	cc $(CFLAGS) -o spew spew.c

install: spew
	cp spew headline /usr/games
