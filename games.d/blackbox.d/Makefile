#
#	$Header: /home/Vince/cvs/games.d/blackbox.d/Makefile,v 1.5 2002-11-24 18:47:28 vrs Exp $ for the black box game
#
ALL	= blackbox$(EXE)
MODEL	= -i -F 2000	# XENIX only
OPTIONS	= -O
include $(HOME)/CONFIG
OBJS	= blackbox.o drawbox.o

blackbox$(EXE): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $@ $(LIBS)

clean:
	rm -f $(OBJS) $(ALL)

install: $(ALL) $(GAMDIR)
	cp $(ALL) $(GAMDIR)
	cd $(GAMDIR); chmod 711 $(ALL)
