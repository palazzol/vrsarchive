#
# sniglets %W%	%G%
#
#
GAMEDIR	= /usr/games
SHELL	= /bin/sh
OBJS	= sniglets.o

sniglets: $(OBJS)
	cc -o sniglets $(OBJS)

lint:
	lint $(OBJS:.o=.c) | tee lint.out

clean:
	rm -f core *.o sniglets a.out

$(GAMEDIR):
	mkdir $@
	chmod 755 $@

install: sniglets $(GAMEDIR)
	cp sniglets $(GAMEDIR)
	chmod 711 $(GAMEDIR)/sniglets
