#
#	%W%	%G%
#
CFLAGS	= -O
LFLAGS	= -lcurses -ltermlib
OBJECT	= curse.o
SHELL	= /bin/sh

curse:	$(OBJECT)
	cc -o $@ $(OBJECT) $(LFLAGS)

clean:
	rm -f *.o curse core make.out
