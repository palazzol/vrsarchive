#
#	$Header: /home/Vince/cvs/local.d/car.d/makefile,v 1.2 1990-03-30 17:02:27 vrs Exp $
#
ALL	= car
MODEL	= -i
OPTIONS	= -O
include /etc/CONFIG

install: $(ALL) $(MAN1) $(BINDIR)
	cp $(ALL) $(BINDIR)
	cd $(BINDIR); chmod 711 $(ALL)
	cp car.1 $(MAN1)
	cd $(MAN1); chmod 644 car.1

clean:
	rm -f $(ALL)
