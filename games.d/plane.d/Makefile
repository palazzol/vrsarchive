#
# plane %W%	%G%
#
#
GAMEDIR	= /usr/games
SHELL	= /bin/sh
LFLAGS	= -F 2000	# XENIX only
OBJS	= plane.o

plane: $(OBJS)
	cc $(LFLAGS) -o plane $(OBJS) -lcurses -ltermlib

lint:
	lint -lcurses -ltermlib $(OBJS:.o=.c) | tee lint.out

clean:
	rm -f core *.o plane a.out

$(GAMEDIR):
	mkdir $@
	chmod 755 $@

install: plane $(GAMEDIR)
	cp plane $(GAMEDIR)
	chmod 711 $(GAMEDIR)/plane
