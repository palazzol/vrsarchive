#
#	$Header: /home/Vince/cvs/local.d/strings.d/makefile,v 1.4 2002-11-24 06:09:40 Vincent Exp $
#
ALL	= strings$(EXE)
MODEL	= -i
OPTIONS	= -O
include $(HOME)/CONFIG

install: $(ALL) $(MAN1) $(BINDIR)
	cp $(ALL) $(BINDIR)
	cd $(BINDIR); chmod 711 $(ALL)
	cp strings.1 $(MAN1)
	cd $(MAN1); chmod 644 strings.1

clean:
	rm -f $(ALL)
