SHELL	= /bin/sh
CFLAGS	= -DRAWMODE -O
OBJS	= hangman.o curses.o doprnt.o
BERKLIB	= ../berkeley.d/Slibberkeley.a
LIBS	= -ltermcap
GAME	= hangman2
GAMES	= cron
GAMDIR	= $(DESTDIR)/usr/games
GAMLIB	= $(GAMDIR)/lib
USRMAN	= $(DESTDIR)/usr/man
MANDIR	= $(USRMAN)/man6

all:	 addword hangman

doprnt.o: $(BERKLIB)
	ar x $? doprnt.S
	mv doprnt.S $@

hangman: $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $@ $(LIBS)

install: addword hangman $(GAMDIR) $(GAMLIB) $(USRMAN) $(MANDIR)
	cp addword $(GAMDIR)
	chown $(GAMES) $(GAMDIR)/addword
	chmod 4711 $(GAMDIR)/addword
	cp hang-words $(GAMLIB)/.hang-words
	chown $(GAMES) $(GAMLIB)/.hang-words
	chmod 644 $(GAMLIB)/.hang-words
	cp hangman $(GAMDIR)/$(GAME)
	chmod 711 $(GAMDIR)/$(GAME)
	cp addword.6 $(MANDIR)
	cp hangman.6 $(MANDIR)/$(GAME).6
	cd $(MANDIR); chmod 644 addword.6 $(GAME).6

$(GAMDIR) $(GAMLIB) $(USRMAN) $(MANDIR):
	mkdir $@
	chmod 755 $@

clean:
	rm -f addword hangman addword.o $(OBJS)
