#
#	corewar %W% %G%
#
SHELL	= /bin/sh
CFLAGS	= -O -Dindex=strchr
GAMDIR	= $(DESTDIR)/usr/games
GAMLIB	= $(GAMDIR)/lib
LIBDIR	= $(GAMLIB)/corewar
USRMAN	= $(DESTDIR)/usr/man
MANDIR	= $(USRMAN)/man6
XMPLES	= backz bigfoot duh dwarf dwarf1 dwarf2 fork_imp \
	  gemini gemini2 imp juggernaut

corewar: corewar.o
	cc -i -o corewar corewar.o

clean:
	rm -f corewar corewar.o

$(GAMDIR) $(GAMLIB) $(LIBDIR) $(USRMAN) $(MANDIR):
	mkdir $@
	chmod 755 $@

install: $(GAMDIR) $(GAMLIB) $(LIBDIR) $(USRMAN) $(MANDIR) corewar
	cp corewar $(GAMDIR)
	chmod 711 $(GAMDIR)/corewar
	cp corewar.6 $(MANDIR)
	chmod 644 $(MANDIR)/corewar.6
	cp $(XMPLES) $(LIBDIR)
	cd $(LIBDIR); chmod 644 $(XMPLES)
