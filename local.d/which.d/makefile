#
#	$Header: /home/Vince/cvs/local.d/which.d/makefile,v 1.3 2002-11-23 07:51:59 Vincent Exp $
#
ALL	= which
MODEL	= -i
OPTIONS	= -O
include $(HOME)/CONFIG

install: $(ALL) $(BINDIR) $(MAN1)
	cp $(ALL) $(BINDIR)
	cd $(BINDIR); chmod 711 $(ALL)
	cp which.1 $(MAN1)
	cd $(MAN1); chmod 644 which.1

clean:
	rm -f $(ALL)
