#
#	$Header: /home/Vince/cvs/local.d/mdemo.d/Makefile,v 1.7 2002-11-23 07:51:53 Vincent Exp $
#
ALL	= mdemo
MODEL	= -M2e -F 2000
include $(HOME)/CONFIG

install:	$(ALL) $(BINDIR)
		cp $(ALL) $(BINDIR)
		cd $(BINDIR); chown root $(ALL)
		cd $(BINDIR); chgrp root $(ALL)
		cd $(BINDIR); chmod 4711 $(ALL)

clean:
		rm -f $(ALL) fluff

lint:
		lint $(LIBS) mdemo.c | tee fluff
