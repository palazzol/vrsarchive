#
# makefile for cxref -- C cross referencing program
#
# Arnold Robbins, Information and Computer Science, Georgia Tech
#	gatech!arnold
# Copyright (c) 1984 by Arnold Robbins.
# All rights reserved.
# This program may not be sold, but may be distributed
# provided this header is included.
#
SCANOBJS = docxref.o cscan.o
SCANSRCS = docxref.c cscan.l

CFLAGS	= -O -Drindex=strrchr
DESTDIR	= /usr/local
LIB	= /usr/local/lib
CXREF 	= cxref
INCLS	= constdefs.h basename.c
PROGS	= docxref fmtxref cxrfilt $(CXREF)
SRCS	= $(SCANSRCS) fmtxref.c cxrfilt.c $(CXREF).c
DOCS	= README makefile cxref.1
PRINTS	= $(INCLS) $(SRCS) $(DOCS)
SHELL	= /bin/sh

all: $(PROGS)
	@echo "	all" done

docxref: $(SCANOBJS)
	cc $(SCANOBJS) -ll -o $@

cscan.o docxref.o cxrfilt.o: $(INCLS)

fmtxref: fmtxref.c
	$(CC) $(CFLAGS) $@.c $(LDFLAGS) -o $@

cxrfilt: cxrfilt.c
	$(CC) $(CFLAGS) $@.c $(LDFLAGS) -o $@

$(CXREF): $(CXREF).c
	$(CC) $(CFLAGS) -DSRCDIR='"$(LIB)"' $(CXREF).c $(LDFLAGS) -o $(CXREF)

print:
	prt $(PRINTS) | lpr -b 'Cxref New Source'
	touch print2

print2: $(PRINTS)
	prt $? | lpr -b 'Cxref New Source'
	touch print2

install: $(PROGS)
# don't remove my bin!
#	rm -fr $(LIB)
	rm -f $(DESTDIR)/cxref
#	mkdir $(LIB)
	cp $(CXREF) $(DESTDIR)/$(CXREF)
	cp docxref  $(LIB)/docxref
	cp fmtxref  $(LIB)/fmtxref
	cp cxrfilt $(LIB)/cxrfilt
#	cp cxref.1 /usr/man/manl/cxref.l
#	cd $(DESTDIR); chmod 711 cxref; chown admin cxref; chgrp admin cxref
#	cd $(LIB); chmod 711 docxref fmtxref cxrfilt
#	cd $(LIB); chown admin docxref fmtxref cxrfilt
#	cd $(LIB); chgrp admin docxref fmtxref cxrfilt

clean:
	rm -f $(SCANOBJS) $(PROGS) core make.out

clobber: clean
	rm -f $(PROGS) print2
