###
# Makefile for the CRACK suite V3.2, (c) ADE Muffett, Aug 1991
###

###
# Configurtable bitz...
###

CC= 		cc
CFLAGS= 	-O
LIBS=		-lnsl
SHELL=		/bin/sh

###
# Not so configurable bitz...
###

PWC= 		crack-pwc
PP= 		crack-pp

###
# There is some redundancy in using CRACKMOD like this, as opposed to a
# library, but it's more portable than trying to use 'ar' - as Chris Lewis
# pointed out...  SWTF. 
###

CRACKLIB= 	crack-lib.o
CRACKMOD= 	crack-fcrypt.o crack-port.o

###
# Default target
###

all:		$(PP) $(PWC)

$(PP):		$(CRACKLIB) crack-pp.o
		$(CC) $(CFLAGS) -o $@ crack-pp.o $(CRACKLIB) $(LIBS)

$(PWC):		$(CRACKLIB) $(CRACKMOD) crack-pwc.o
		$(CC) $(CFLAGS) -o $@ crack-pwc.o $(CRACKLIB) $(CRACKMOD) $(LIBS)

crack-pp.c: 	crack.h

crack-pwc.c:	crack.h

crack-lib.c:	crack.h

crack.h:	conf.h
		touch crack.h
conf.h:

clean:
		-rm -f $(PP) $(PWC) *.o *.u *.a
		-rm -f tester *.pixie *.Addrs *.Counts
		-rm -f speedcrypt speedfcrypt speedxform

###
# These targets below are for fcrypt() development only
###

PIXIEF = -procedures -heavy -invocations -zero

tests:		speedcrypt tester speedxform speedfcrypt
		-./speedcrypt
		-./speedfcrypt
		-./speedxform
		-./tester

speedfcrypt:	speeds.c $(CRACKLIB) $(CRACKMOD)
		$(CC) -DT1 -o speedfcrypt speeds.c $(CRACKLIB) $(CRACKMOD)

speedxform:	speeds.c $(CRACKLIB) $(CRACKMOD)
		$(CC) -DT2 -o speedxform speeds.c $(CRACKLIB) $(CRACKMOD)

speedcrypt:	speeds.c $(CRACKLIB) $(CRACKMOD)
		$(CC) -o speedcrypt speeds.c $(CRACKLIB) $(CRACKMOD)

tester:		tester.o $(CRACKLIB) $(CRACKMOD)
		$(CC) -o tester tester.o $(CRACKLIB) $(CRACKMOD)

tester.pixie:	tester
		pixie -o tester.pixie tester

prof:		tester.pixie
		time tester.pixie
		prof -pixie tester $(PIXIEF)

b:	crack-fcrypt.c.orig

crack-fcrypt.c.orig:	crack-fcrypt.c
	cp crack-fcrypt.c crack-fcrypt.c.orig

