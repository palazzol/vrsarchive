#
#	Robots %W%
#
ALL	= robots
OPTIONS	= -DALLSCORES -DOLDSCORES -DGAMLIB=$(GAMLIB)
MODEL	= -i -F 2000	# Required for XENIX only
include /etc/CONFIG

install:	$(ALL) $(GAMDIR) $(GAMLIB) $(MAN6)
		cp $(ALL) $(GAMDIR)
		cd $(GAMDIR); chmod 4711 $(ALL)
		cd $(GAMLIB); touch robots_hof
		cd $(GAMLIB); touch robots_tmp
		cp robots.6 $(MAN6)/robots.6
		chmod 644 $(MAN6)/robots.6

clean:
		rm -f $(ALL)

lint:
		lint -lcurses -ltermcap robots.c | tee lint.out
