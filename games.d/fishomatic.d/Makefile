#
#	$Header: /home/Vince/cvs/games.d/fishomatic.d/Makefile,v 1.2 1987-06-08 21:25:35 vrs Exp $
#
ALL	= fishomatic
MODEL	= -i
OPTIONS	= -DFISH=\"$(GAMDIR)/fish\"
include ../../CONFIG

lint:
	lint $(OBJS:.o=.c) | tee lint.out

clean:
	rm -f $(ALL)

install: $(ALL) $(GAMDIR)
	cp $(ALL) $(GAMDIR)
	cd $(GAMDIR); chmod 711 $(ALL)
