# -*- Mode: Text -*-

# Look over config.X before building.
#
# You may want to edit BINDIR, LIBDIR, DEFHASH, DEFDICT, MAN1DIR, MAN4DIR
# MAN1EXT, MAN4EXT, and TERMLIB below;
# the Makefile will update all other files to match.
#
# On USG systems, add -DUSG to CFLAGS.  On BSD, remove it.
#
# The ifdef NO8BIT may be used if 8 bit extended text characters
# cause problems, or you simply don't wish to allow the feature.
#
# the argument syntax for buildhash to make alternate dictionary files
# is simply:
#
#   buildhash <infile> <outfile>

CC = cc
CFLAGS = -n -O -DUSG
# BINDIR, LIBDIR, DEFHASH, DEFDICT, MAN1DIR, MAN4DIR, MAN1EXT, MAN4EXT,
# TERMLIB
BINDIR = /usr/sahbin
LIBDIR = /tmp2/lib
DEFHASH = ispell.hash
DEFDICT = dict.191
MAN1DIR	= /usr/man/u_man/man1
MAN4DIR	= /usr/man/u_man/man4
MAN1EXT	= .1l
MAN4EXT	= .4l
# TERMLIB = -lcurses
TERMLIB = -ltermcap

SHELL = /bin/sh

all: buildhash fixdict ispell icombine munchlist isexpand $(DEFHASH)

ispell.hash: buildhash $(DEFDICT)
	./buildhash $(DEFDICT) $(DEFHASH)

install: all
	cp ispell isexpand munchlist $(BINDIR)
	cp ispell.hash $(LIBDIR)/$(DEFHASH)
	cp isexp[1-4].sed icombine $(LIBDIR)
	chmod 755 $(BINDIR)/ispell $(BINDIR)/munchlist $(BINDIR)/isexpand \
	  $(LIBDIR)/icombine
	chmod 644 $(LIBDIR)/$(DEFHASH) $(LIBDIR)/isexp[1-4].sed
	cp ispell.1 $(MAN1DIR)/ispell$(MAN1EXT)
	cp ispell.4 $(MAN4DIR)/ispell$(MAN4EXT)

buildhash: buildhash.o hash.o
	$(CC) $(CFLAGS) -o buildhash buildhash.o hash.o

fixdict:	fixdict.X Makefile
	sed -e 's@!!LIBDIR!!@$(LIBDIR)@' -e 's@!!DEFDICT!!@$(DEFDICT)@' \
		<fixdict.X >fixdict
	chmod +x fixdict

icombine:	icombine.c config.h ispell.h
	$(CC) $(CFLAGS) -o icombine icombine.c

munchlist:	munchlist.X Makefile
	sed -e 's@!!LIBDIR!!@$(LIBDIR)@' -e 's@!!DEFDICT!!@$(DEFDICT)@' \
		<munchlist.X >munchlist
	chmod +x munchlist

isexpand:	isexpand.X Makefile
	sed -e 's@!!LIBDIR!!@$(LIBDIR)@' isexpand.X >isexpand
	chmod +x isexpand

OBJS=ispell.o term.o good.o lookup.o hash.o tree.o xgets.o

ispell: $(OBJS)
	cc $(CFLAGS) -o ispell $(OBJS) $(TERMLIB)

$(OBJS) buildhash.o: config.h ispell.h
ispell.o: version.h

config.h:	config.X Makefile
	sed -e 's@!!LIBDIR!!@$(LIBDIR)@' -e 's@!!DEFDICT!!@$(DEFDICT)@' \
	    -e 's@!!DEFHASH!!@$(DEFHASH)@' <config.X >config.h

clean:
	rm -f *.o buildhash ispell core a.out mon.out hash.out \
		*.stat *.cnt fixdict fixdict.sh icombine munchlist config.h
