#
#	%W% for the black box game %G%
#
GAMEDIR	= /usr/games
LFLAGS	= -F 2000	# XENIX only
LIBS	= -lcurses -ltermcap
SHELL	= /bin/sh

blackbox: blackbox.o drawbox.o
	cc $(LFLAGS) blackbox.o drawbox.o -o $@ $(LIBS)

clean:
	rm -f *.o blackbox core make.out

$(GAMEDIR):
	mkdir $@
	chmod 755 $@

install: blackbox $(GAMEDIR)
	cp blackbox $(GAMEDIR)
	chmod 711 $(GAMEDIR)/blackbox
