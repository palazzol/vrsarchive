#
#	Robots %W%
#
CFLAGS	= -DALLSCORES -DOLDSCORES
LFLAGS	= -F 2000	# Required for XENIX only
LIBS	= -lx		# Required for XENIX only
GAMEDIR	= /usr/games
MAN	= /usr/man
MANDIR	= $(MAN)/man6
SHELL	= /bin/sh

robots:		robots.o
		cc $(LFLAGS) robots.o -o robots -lcurses -ltermcap $(LIBS)

$(MAN):
		mkdir $@
		chmod 755 $@

$(MANDIR):	$(MAN)
		mkdir $@
		chmod 755 $@

$(GAMEDIR):
		mkdir $@
		chmod 755 $@

$(GAMEDIR)/lib:
		mkdir $@
		chmod 755 $@

install:	$(GAMEDIR) $(GAMEDIR)/lib $(MANDIR) robots
		cp robots.6 $(MANDIR)/robots.6
		chmod 644 $(MANDIR)/robots.6
		cp robots $(GAMEDIR)/robots
		chmod 4711 $(GAMEDIR)/robots
		touch $(GAMEDIR)/lib/robots_hof
		touch $(GAMEDIR)/lib/robots_tmp

clean:
		rm -f robots *.o

lint:
		lint -lcurses -ltermcap robots.c | tee lint.out
