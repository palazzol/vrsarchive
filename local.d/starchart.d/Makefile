#
# Author: Petri Launiainen, Intrinsic Oy, Tampere, FINLAND (pl@intrin.FI)
#
# Based on the work of Alan W. Paeth
#

SHELL = /bin/sh

#
# put the Yale Star Catalog file path here: because the yale.star is
# static (stars don't change so often :->), consider using some static
# library path here - relative one is used for quick testing

#STAR = "/usr/lib/starchart/yale.star"
STAR = "./yale.star"

# similarly assign the Messier file path:

#MESS = "/usr/lib/starchart/messier.star"
MESS = "./messier.star"

# put the planet position file path here: it should be preferably
# relative, thus allowing everyone to change planet positions according
# to their interests.  This file changes when the 'planet' program is
# executed

PLANET = "./planet.star"

# notice that starchart.o and planet are made dependent on Makefile, due
# to the STAR, MESS and PLANET definitions here

# put the installation directory here

BINDIR = "/usr/local/bin"

CFLAGS = -O -DSTARFILE='$(STAR)' -DPLANETFILE='$(PLANET)' -DMESSFILE='$(MESS)'
LFLAGS = -O

# star chart calculation routine file

COBJ = starchart.o

# default binary names: if you change these, change also the corresponding
# make tags

BINS = epoch moonphase planet startek stardsp starpic starhp starlaser starpost






test:
	stardsp -c ori

all:	\
	planet\
	starhp\
	stardsp\
	startek\
	starpic\
	starpost\
	starlaser\
	epoch\
	moonphase

install: all
	strip $(BINS)
	mv $(BINS) $(BINDIR)

starchart.o: starchart.c Makefile starchart.h

moonphase: moonphase.o
	cc $(LFLAGS) moonphase.o -lm -o $@

epoch: epoch.o
	cc $(LFLAGS) epoch.o -lm -o $@

planet:	planet.o Makefile
	cc $(LFLAGS) $(DEFINES) planet.o -lm -o $@

stardsp: $(COBJ) stardsp.o
	cc $(LFLAGS) $(COBJ) stardsp.o -lm -o $@

starpic: $(COBJ) starpic.o
	cc $(LFLAGS) $(COBJ) starpic.o -lm -o $@

startek: $(COBJ) startek.o starimages.o
	cc $(LFLAGS) $(COBJ) startek.o starimages.o -lm -o $@

starhp: $(COBJ) starhp.o starimages.o
	cc $(LFLAGS) $(COBJ) starhp.o starimages.o -lm -o $@

starlaser: $(COBJ) starlaser.o starimages.o
	cc $(LFLAGS) $(COBJ) starlaser.o starimages.o -lm -o $@

starpost: $(COBJ) starpost.o
	cc $(LFLAGS) $(COBJ) starpost.o -lm -o $@

make clean:
	rm -f *.o mlog core
