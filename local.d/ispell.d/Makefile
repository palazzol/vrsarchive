# -*- Mode: Text -*-

# Look over config.X before building.
#
# You may want to edit BINDIR, LIBDIR, DEFHASH, DEFDICT, MAN1DIR, MAN4DIR
# MAN1EXT, MAN4EXT, and LIBS below;
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
OPTIONS = -O -DUSG
ALL	= ispell.hash fixdict ispell icombine munchlist isexpand $(DEFHASH)
include ../../CONFIG
# BINDIR, LIBDIR, DEFHASH, DEFDICT, MAN1DIR, MAN4DIR, MAN1EXT, MAN4EXT,
# LIBS
DEFHASH = ispell.hash
DEFDICT = dict.191
MAN1DIR	= $(MAN1)
MAN4DIR	= $(MAN4)
MAN1EXT	= .1
MAN4EXT	= .4

ispell.hash: buildhash $(DEFDICT)
	./buildhash $(DEFDICT) $(DEFHASH)

install: $(ALL) $(MAN1DIR) $(MAN4DIR)
	cp ispell isexpand munchlist $(BINDIR)
	cd $(BINDIR); chmod 755 isexpand munchlist
	cd $(BINDIR); chmod 711 ispell
	cp ispell.hash $(LIBDIR)/$(DEFHASH)
	cp isexp[1-4].sed icombine $(LIBDIR)
	cd $(LIBDIR); chmod 711 icombine
	cd $(LIBDIR); chmod 644 $(DEFHASH) isexp[1-4].sed
	cp ispell.1 $(MAN1DIR)/ispell$(MAN1EXT)
	chmod 644 $(MAN1DIR)/ispell$(MAN1EXT)
	cp ispell.4 $(MAN4DIR)/ispell$(MAN4EXT)
	chmod 644 $(MAN4DIR)/ispell$(MAN4EXT)

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
	cc $(CFLAGS) -o ispell $(OBJS) $(LIBS)

$(OBJS) buildhash.o: config.h ispell.h
ispell.o: version.h

config.h:	config.X Makefile
	sed -e 's@!!LIBDIR!!@$(LIBDIR)@' -e 's@!!DEFDICT!!@$(DEFDICT)@' \
	    -e 's@!!DEFHASH!!@$(DEFHASH)@' <config.X >config.h

clean:
	rm -f *.o buildhash ispell core a.out mon.out hash.out isexpand *.hash
	rm -f *.stat *.cnt fixdict fixdict.sh icombine munchlist config.h
