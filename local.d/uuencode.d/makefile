#
#	$Header: /home/Vince/cvs/local.d/uuencode.d/makefile,v 1.2 1990-03-30 17:03:45 vrs Exp $
#
ALL	= uuencode uudecode
MODEL	= -i
OPTIONS	= -O
include /etc/CONFIG

install: $(ALL) $(BINDIR)
	cp $(ALL) $(BINDIR)
	cd $(BINDIR); chmod 711 $(ALL)
	cp uuencode.1 $(MAN1)/uuencode.1
	cp uuencode.1 $(MAN1)/uudecode.1
	chmod 644 $(MAN1)/uuencode.1 $(MAN1)/uudecode.1

clean:
	rm -f $(ALL) lint.out tags
