#
#	$Header: /home/Vince/cvs/local.d/copymode.d/makefile,v 1.2 1990-03-30 17:02:32 vrs Exp $
#
ALL	= copymode
MODEL	= -i
OPTIONS	= -O
include /etc/CONFIG

install: $(ALL) $(MAN1) $(BINDIR)
	cp $(ALL) $(BINDIR)
	cd $(BINDIR); chmod 711 $(ALL)
	cp copymode.1 $(MAN1)
	cd $(MAN1); chmod 644 copymode.1

clean:
	rm -f $(ALL)
