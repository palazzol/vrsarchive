#
#	life %W% %G%
#
SHELL	= /bin/sh
SOURCES	= life.c
OBJECTS	= life.o
CFLAGS	= -F 2000
GAMDIR	= $(DESTDIR)/usr/games

life: $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $(OBJECTS) -lcurses -ltermcap

clean:
	rm -f $(OBJECTS) life core

$(GAMDIR):
	mkdir $@
	chmod 755 $@

install: $(GAMDIR) life
	cp life $(GAMDIR)/life
	chmod 711 $(GAMDIR)/life
