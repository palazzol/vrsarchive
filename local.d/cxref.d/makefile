#
# $Header: /home/Vince/cvs/local.d/cxref.d/makefile,v 1.3 1990-03-30 17:02:41 vrs Exp $
# makefile for cxref -- C cross referencing program
#
# Arnold Robbins, Information and Computer Science, Georgia Tech
#	gatech!arnold
# Copyright (c) 1984 by Arnold Robbins.
# All rights reserved.
# This program may not be sold, but may be distributed
# provided this header is included.
#
ALL	= cxref docxref fmtxref cxrfilt
MODEL	= -i
OPTIONS	= -O -DSRCDIR='"$(LIBDIR)"'
include /etc/CONFIG

SCANOBJS = docxref.o cscan.o
SCANSRCS = docxref.c cscan.l
INCLS	= constdefs.h basename.c
SRCS	= $(SCANSRCS) cxref.c fmtxref.c cxrfilt.c
DOCS	= README makefile cxref.1
PRINTS	= $(INCLS) $(SRCS) $(DOCS)

docxref: $(SCANOBJS)
	cc $(SCANOBJS) -ll -o $@

cscan.o docxref.o cxrfilt.o: $(INCLS)

install: $(ALL) $(BINDIR) $(LIBDIR) $(MAN1)
	cp cxref $(BINDIR)
	cd $(BINDIR); chmod 711 cxref
	cp cxrfilt docxref fmtxref $(LIBDIR)
	cd $(LIBDIR); chmod 711 cxrfilt docxref fmtxref
	cp cxref.1 $(MAN1)
	cd $(MAN1); chmod 644 cxref.1

clean:
	rm -f $(SCANOBJS) $(ALL) core make.out
