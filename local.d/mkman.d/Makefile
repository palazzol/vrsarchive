#
#	$Header: /home/Vince/cvs/local.d/mkman.d/Makefile,v 1.3 2002-11-23 07:51:54 Vincent Exp $
#
ALL	= mkman
MODEL	= -i
OPTIONS	= -O
include $(HOME)/CONFIG

install: $(ALL) $(BINDIR)
	cp $(ALL) $(BINDIR)
	cd $(BINDIR); chmod 711 $(ALL)

clean:
	rm -f $(ALL)
