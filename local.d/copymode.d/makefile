#
#	$Header: /home/Vince/cvs/local.d/copymode.d/makefile,v 1.3 2002-11-23 07:49:03 Vincent Exp $
#
ALL	= copymode
MODEL	= -i
OPTIONS	= -O
include $(HOME)/CONFIG

install: $(ALL) $(MAN1) $(BINDIR)
	cp $(ALL) $(BINDIR)
	cd $(BINDIR); chmod 711 $(ALL)
	cp copymode.1 $(MAN1)
	cd $(MAN1); chmod 644 copymode.1

clean:
	rm -f $(ALL)
