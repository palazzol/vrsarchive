#
#	robomatic @(#)Makefile	1.7 10/19/85
#
ALL	= robomatic
OPTIONS	= -O -DGAMDIR=\"$(GAMDIR) -DGAMLIB=\"$(GAMLIB)
MODEL	= -i -F 2000
include /etc/CONFIG

install: $(ALL) $(GAMDIR) $(MAN6)
	cp $(ALL) $(GAMDIR)
	cd $(GAMDIR); chmod 711 $(ALL)
	cp robomatic.6 $(MAN6)
	chmod 644 $(MAN6)/robomatic.6

lint:
	lint $(CFLAGS) $(LIBS) robomatic.c | tee lint.out

clean:
	rm -f $(ALL) lint.out core
