#
#	$Header: /home/Vince/cvs/local.d/diffc.d/makefile,v 1.4 2002-11-24 05:09:10 Vincent Exp $
#
MODEL	= -i
OPTIONS	= -O
ALL	= diffc$(EXE)
include $(HOME)/CONFIG

install: $(ALL) $(BINDIR) $(MAN1)
	cp $(ALL) $(BINDIR)
	cd $(BINDIR); chmod 711 $(ALL)
	cp diffc.1 $(MAN1)
	cd $(MAN1); chmod 644 diffc.1

clean:
	rm -f $(ALL)
