#
#	%W%	%G%
#
CFLAGS	= -O -Dindex=strchr
LFLAGS	= -F 2000
GAMEDIR	= /usr/games
SHELL	= /bin/sh
OBJS	= endgame.o extern.o getguess.o getword.o main.o playgame.o \
	  prdata.o prman.o prword.o setup.o
CFILES	= endgame.c extern.c getguess.c getword.c main.c playgame.c \
	  prdata.c prman.c prword.c setup.c
HDRS	= hangman.h

all: hangman

tags: $(HDRS) $(CFILES)
	ctags $(HDRS) $(CFILES)

install: hangman $(GAMEDIR)
	cp hangman $(GAMEDIR)/hangman
	chmod 711 $(GAMEDIR)/hangman
	
hangman: $(OBJS)
	$(CC) $(LFLAGS) -o hangman $(OBJS) -lcurses -ltermlib

clean:
	rm -f $(OBJS) hangman core make.out

$(GAMEDIR):
	mkdir $@
	chmod 755 $@
