#
#	$Header: /home/Vince/cvs/games.d/fishomatic.d/Makefile,v 1.3 1990-03-27 17:25:23 vrs Exp $
#
ALL	= fishomatic
MODEL	= -i
OPTIONS	= -DFISH=\"$(GAMDIR)/fish\"
include /etc/CONFIG

lint:
	lint $(OBJS:.o=.c) | tee lint.out

clean:
	rm -f $(ALL)

install: $(ALL) $(GAMDIR)
	cp $(ALL) $(GAMDIR)
	cd $(GAMDIR); chmod 711 $(ALL)
