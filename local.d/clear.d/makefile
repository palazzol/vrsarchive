#
#	%W%	%G%
#
CFLAGS	= -O
LFLAGS	= -lcurses -ltermlib
OBJECT	= clear.o
SHELL	= /bin/sh

clear:	$(OBJECT)
	cc -o $@ $(OBJECT) $(LFLAGS)

clean:
	rm -f *.o clear core make.out
