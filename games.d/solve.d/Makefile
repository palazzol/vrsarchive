#
#	%W%	%G%
#
CFLAGS	= -O
GAMEDIR	= /usr/games
USRMAN	= /usr/man
MANDIR	= $(USRMAN)/man6
SHELL	= /bin/sh
OBJS	= solve.o
CFILES	= solve.c

all: solve

tags:	$(CFILES)
	ctags $(CFILES)

install: solve $(GAMEDIR) $(USRMAN) $(MANDIR)
	cp solve $(GAMEDIR)/solve
	chmod 711 $(GAMEDIR)/solve
	cp solve.6 $(MANDIR)/solve.6
	chmod 644 $(MANDIR)/solve.6

solve: $(OBJS)
	$(CC) -o solve $(OBJS)

clean:
	rm -f $(OBJS) solve core make.out

$(GAMEDIR) $(USRMAN) $(MANDIR):
	mkdir $@
	chmod 755 $@
