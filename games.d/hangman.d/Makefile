#
#	$Header: /home/Vince/cvs/games.d/hangman.d/Makefile,v 1.5 1990-03-27 17:25:43 vrs Exp $
#
ALL	= hangman
MODEL	= -i -F 2000
OPTIONS	= -O -DGAMLIB=\"$(GAMLIB)
include /etc/CONFIG
OBJS	= endgame.o extern.o getguess.o getword.o main.o playgame.o \
	  prdata.o prman.o prword.o setup.o
CFILES	= endgame.c extern.c getguess.c getword.c main.c playgame.c \
	  prdata.c prman.c prword.c setup.c
HDRS	= hangman.h

tags: $(HDRS) $(CFILES)
	ctags $(HDRS) $(CFILES)

install: $(ALL) $(GAMDIR) $(GAMLIB) $(MAN6)
	cp $(ALL) $(GAMDIR)
	cd $(GAMDIR); chmod 711 $(ALL)
	cp words $(GAMLIB)
	cd $(GAMLIB); chmod 644 words
	cp hangman.6 $(MAN6)
	cd $(MAN6); chmod 644 hangman.6
	
hangman: $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS) $(LIBS)

clean:
	rm -f $(OBJS) $(ALL)

$(OBJS): $(HDRS)
