#
#	$Header: /home/Vince/cvs/local.d/mdemo.d/Makefile,v 1.6 1990-03-30 17:03:04 vrs Exp $
#
ALL	= mdemo
MODEL	= -M2e -F 2000
include /etc/CONFIG

install:	$(ALL) $(BINDIR)
		cp $(ALL) $(BINDIR)
		cd $(BINDIR); chown root $(ALL)
		cd $(BINDIR); chgrp root $(ALL)
		cd $(BINDIR); chmod 4711 $(ALL)

clean:
		rm -f $(ALL) fluff

lint:
		lint $(LIBS) mdemo.c | tee fluff
