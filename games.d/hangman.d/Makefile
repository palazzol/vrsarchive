#
#	$Header: /home/Vince/cvs/games.d/hangman.d/Makefile,v 1.2 1987-06-08 21:25:57 vrs Exp $
#
ALL	= hangman
MODEL	= -i -F 2000
OPTIONS	= -O
include ../../CONFIG
OBJS	= endgame.o extern.o getguess.o getword.o main.o playgame.o \
	  prdata.o prman.o prword.o setup.o
CFILES	= endgame.c extern.c getguess.c getword.c main.c playgame.c \
	  prdata.c prman.c prword.c setup.c
HDRS	= hangman.h

tags: $(HDRS) $(CFILES)
	ctags $(HDRS) $(CFILES)

install: $(ALL) $(GAMDIR) $(MAN6)
	cp $(ALL) $(GAMDIR)
	cd $(GAMDIR); chmod 711 $(ALL)
	cp hangman.6 $(MAN6)
	cd $(MAN6); chmod 711 hangman.6
	
hangman: $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS) -lcurses -ltermlib

clean:
	rm -f $(OBJS) $(ALL)

$(OBJS): $(HDRS)
