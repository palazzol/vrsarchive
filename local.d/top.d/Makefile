#
#	@(#)Makefile	1.5	3/30/85
#
# values for two defaults in "top"
TOPN = 10
DELAY = 5

# various other useful things
MAN	= man
SHELL	= /bin/sh
TROFF	= itroff
CFLAGS	= -O

top: top.o bzero.o
	cc $(CFLAGS) -o top top.o bzero.o -lcurses -ltermcap -lm

top.o: top.c
	cc -c $(CFLAGS) -DDefault_TOPN=$(TOPN) -DDefault_DELAY=$(DELAY) top.c

clean:
	rm -f top *.o make.out

man:
	nroff -$(MAN) top.1 | cat -s >top.cat

troff:
	$(TROFF) -man top.1

tar:	clean
	tar cvf top.tar *
