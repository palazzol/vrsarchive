#
# plane %W%	%G%
#
ALL	= plane
MODEL	= -i -F 2000	# XENIX only
OPTIONS	= -O
include /etc/CONFIG

lint:
	lint -lcurses -ltermlib $(OBJS:.o=.c) | tee lint.out

clean:
	rm -f $(ALL)

install: $(ALL) $(GAMDIR)
	cp $(ALL) $(GAMDIR)
	cd $(GAMDIR); chmod 711 $(ALL)
