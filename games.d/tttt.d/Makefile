# makefile for tic-tac-toc-toe
#
# edit as necessary, then make

# where the executable will go
BINDIR = /usr/games

# where the formatted manpage will go
MANDIR = /usr/games/Doc

# options for the c compiler (-g, -O, etc.)
CFLAGS = -O

# compiler libraries (-ltermcap, etc.)
LIBS = -lcurses -ltermcap

tttt: tttt.c
	cc $(CFLAGS) -o tttt tttt.c $(LIBS)

install: tttt tttt.man
	cp tttt $(BINDIR)/tttt
	chmod 711 $(BINDIR)/tttt
	cp tttt.man $(MANDIR)/tttt.6
	chmod 644 $(MANDIR)/tttt.6

tttt.man: tttt.6
	nroff -man tttt.6 > tttt.man

clean:
	rm -f tttt *.o tttt.man tttt.shar

shar:
	shar README Makefile tttt.6 tttt.c > tttt.shar
