#
#	$Header: /home/Vince/cvs/games.d/blackbox.d/Makefile,v 1.3 1990-03-27 17:24:47 vrs Exp $ for the black box game
#
ALL	= blackbox
MODEL	= -i -F 2000	# XENIX only
OPTIONS	= -O
include /etc/CONFIG
OBJS	= blackbox.o drawbox.o

blackbox: $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $@ $(LIBS)

clean:
	rm -f $(OBJS) $(ALL)

install: $(ALL) $(GAMDIR)
	cp $(ALL) $(GAMDIR)
	cd $(GAMDIR); chmod 711 $(ALL)
