#
#	$Header: /home/Vince/cvs/local.d/car.d/makefile,v 1.4 2002-11-24 03:27:04 Vincent Exp $
#
ALL	= car$(EXE)
MODEL	= -i
OPTIONS	= -O
include $(HOME)/CONFIG

install: $(ALL) $(MAN1) $(BINDIR)
	cp $(ALL) $(BINDIR)
	cd $(BINDIR); chmod 711 $(ALL)
	cp car.1 $(MAN1)
	cd $(MAN1); chmod 644 car.1

clean:
	rm -f $(ALL)
