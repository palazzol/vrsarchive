#
#	%W%	%G%
#
BIN	= /usr/local
# if you have bugs in your C compiler dont use -O
#CFLAGS	= -DBSD4_2 -O -DSACREDMEM=256000 -DUSERMEM=0
CFLAGS	= -O -DSHORT_INT
LFLAGS	= -i -F 3000
SHELL	= /bin/sh

compress : compress.o
	cc -o compress compress.o $(LFLAGS)

compress.o: compress.c USERMEM
	cc -c $(CFLAGS) compress.c

# USERMEM may have to be set by hand.  It should contain the amount of
# available user memory in bytes.  Set it to zero, for physical memory
# less than 1 Meg.
USERMEM:
	sh usermem > USERMEM

/usr/man $(BIN):
		mkdir $@

/usr/man/manl:	/usr/man
		mkdir /usr/man/manl

install: compress /usr/man/manl
	cp compress $(BIN)/compress
	cp compress.l /usr/man/manl
	rm -f $(BIN)/uncompress $(BIN)/zcat
	ln $(BIN)/compress $(BIN)/uncompress
	ln $(BIN)/compress $(BIN)/zcat

clean:
	rm -f *.o core Make.out compress uncompress zcat
