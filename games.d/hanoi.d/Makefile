#
# hanoi %W%	%G%
#
#
GAMEDIR	= /usr/games
SHELL	= /bin/sh
LFLAGS	= -F 2000	# XENIX only
OBJS	= hanoi.o

hanoi: $(OBJS)
	cc $(LFLAGS) -o hanoi $(OBJS) -lcurses -ltermlib

lint:
	lint -lcurses -ltermlib $(OBJS:.o=.c) | tee lint.out

clean:
	rm -f core *.o hanoi a.out

$(GAMEDIR):
	mkdir $@
	chmod 755 $@

install: hanoi $(GAMEDIR)
	cp hanoi $(GAMEDIR)
	chmod 711 $(GAMEDIR)/hanoi
