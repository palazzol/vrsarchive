#
# fishomatic %W%	%G%
#
#
GAMEDIR	= /usr/games
SHELL	= /bin/sh
OBJS	= fishomatic.o

fishomatic: $(OBJS)
	cc -o fishomatic $(OBJS)

lint:
	lint $(OBJS:.o=.c) | tee lint.out

clean:
	rm -f core *.o fishomatic a.out

$(GAMEDIR):
	mkdir $@
	chmod 755 $@

install: fishomatic $(GAMEDIR)
	cp fishomatic $(GAMEDIR)
	chmod 711 $(GAMEDIR)/fishomatic
