# -*- Mode: Text -*-

# Look over config.h before building.
#
# LIBDIR, DEFHASH, DEFDICT should match definitions in config.h.
#
# The ifdef NO8BIT may be used if 8 bit extended text characters
# cause problems, or you simply don't wish to allow the feature.
#
# the argument syntax for buildhash to make alternate dictionary files
# is simply:
#
#   buildhash <infile> <outfile>

CFLAGS = -O
BINDIR = /usr/local/bin
LIBDIR = /usr/local/lib
DEFHASH = ispell.hash
DEFDICT = dict.191

# TERMLIB = -lcurses
TERMLIB = -ltermlib
all: buildhash ispell $(DEFHASH)

ispell.hash: buildhash $(DEFDICT)
	buildhash

install: buildhash ispell $(DEFHASH)
	cp ispell ${BINDIR}/ispell
	cp munchlist.sh $(BINDIR)/munchlist
	cp ispell.hash ${LIBDIR}/${DEFHASH}
	cp expand1.sed expand2.sed $(LIBDIR)
	chmod 755 ${BINDIR}/ispell $(BINDIR)/munchlist
	chmod 644 ${LIBDIR}/$(DEFHASH) $(LIBDIR)/expand1.sed \
	  $(LIBDIR)/expand2.sed

buildhash: buildhash.o hash.o
	cc -o buildhash buildhash.o hash.o

ispell: ispell.o term.o good.o lookup.o hash.o tree.o
	cc $(CFLAGS) -o ispell ispell.o term.o good.o lookup.o \
		hash.o tree.o $(TERMLIB)

clean:
	rm -f *.o buildhash ispell core a.out mon.out hash.out \
		*.stat *.cnt
