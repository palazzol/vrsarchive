#
#	$Header: /home/Vince/cvs/games.d/fishomatic.d/Makefile,v 1.4 2002-11-23 04:09:38 Vincent Exp $
#
ALL	= fishomatic$(EXE)
MODEL	= -i
OPTIONS	= -DFISH=\"$(GAMDIR)/fish\"
include $(HOME)/CONFIG

fishomatic$(EXE): fishomatic.c
	$(CC) $(CFLAGS) -o $@ $<
lint:
	lint $(OBJS:.o=.c) | tee lint.out

clean:
	rm -f $(ALL)

install: $(ALL) $(GAMDIR)
	cp $(ALL) $(GAMDIR)
	cd $(GAMDIR); chmod 711 $(ALL)
