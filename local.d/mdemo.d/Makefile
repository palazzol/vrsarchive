#
#	$Header: /home/Vince/cvs/local.d/mdemo.d/Makefile,v 1.5 1987-02-15 15:54:39 vrs Exp $
#
ALL	= mdemo
MODEL	= -M2e -F 2000
include ../../CONFIG

install:	$(ALL) $(BINDIR)
		cp $(ALL) $(BINDIR)
		cd $(BINDIR); chown root $(ALL)
		cd $(BINDIR); chgrp root $(ALL)
		cd $(BINDIR); chmod 4711 $(ALL)

clean:
		rm -f $(ALL) fluff

lint:
		lint $(LIBS) mdemo.c | tee fluff
