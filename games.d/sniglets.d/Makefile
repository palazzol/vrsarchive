#
# sniglets %W%	%G%
#
#
ALL	= sniglets
include ../../CONFIG

lint:
	lint sniglets.c | tee lint.out

clean:
	rm -f core $(ALL)

install: $(ALL) $(GAMDIR)
	cp $(ALL) $(GAMDIR)
	cd $(GAMDIR); chmod 711 $(ALL)
