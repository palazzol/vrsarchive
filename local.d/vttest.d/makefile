#
#	$Header: /home/Vince/cvs/local.d/vttest.d/makefile,v 1.4 2002-11-23 07:51:59 Vincent Exp $
#
ALL	= vttest
MODEL	= -i
OPTIONS	= -O
include $(HOME)/CONFIG

install: $(ALL)
	cp $(ALL) $(BINDIR)
	cd $(BINDIR); chmod 711 $(ALL)

clean:
	rm -f $(ALL)
