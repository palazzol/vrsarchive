#
#	$Header: /home/Vince/cvs/games.d/hangman2.d/Makefile,v 1.2 1987-06-08 21:26:07 vrs Exp $
#
ALL	= addword hangman2
MODEL	= -i
OPTIONS	= -DGAMLIB=\"$(GAMLIB) -DRAWMODE -O
include ../../CONFIG
OBJS	= hangman.o curses.o doprnt.o
GAME	= hangman2

hangman2: $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $@ $(LIBS)

install: $(ALL) $(GAMDIR) $(GAMLIB) $(MAN6)
	cp $(ALL) $(GAMDIR)
	cd $(GAMDIR); chown $(SAFEUID) $(ALL)
	cd $(GAMDIR); chmod 4711 $(ALL)
	cp hang-words $(GAMLIB)/.hang-words
	chown $(SAFEUID) $(GAMLIB)/.hang-words
	chmod 644 $(GAMLIB)/.hang-words
	cp addword.6 $(MAN6)
	cp hangman.6 $(MAN6)/$(GAME).6
	cd $(MAN6); chmod 644 addword.6 $(GAME).6

clean:
	rm -f $(ALL) $(OBJS)
