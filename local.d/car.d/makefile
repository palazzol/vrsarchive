#
#	$Header: /home/Vince/cvs/local.d/car.d/makefile,v 1.1 1987-02-15 16:05:53 vrs Exp $
#
ALL	= car
MODEL	= -i
OPTIONS	= -O
include ../../CONFIG

install: $(ALL) $(MAN1) $(BINDIR)
	cp $(ALL) $(BINDIR)
	cd $(BINDIR); chmod 711 $(ALL)
	cp car.1 $(MAN1)
	cd $(MAN1); chmod 644 car.1

clean:
	rm -f $(ALL)
