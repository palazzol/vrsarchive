#
#	$Header: /home/Vince/cvs/local.d/uuencode.d/makefile,v 1.3 2002-11-23 07:51:58 Vincent Exp $
#
ALL	= uuencode uudecode
MODEL	= -i
OPTIONS	= -O
include $(HOME)/CONFIG

install: $(ALL) $(BINDIR)
	cp $(ALL) $(BINDIR)
	cd $(BINDIR); chmod 711 $(ALL)
	cp uuencode.1 $(MAN1)/uuencode.1
	cp uuencode.1 $(MAN1)/uudecode.1
	chmod 644 $(MAN1)/uuencode.1 $(MAN1)/uudecode.1

clean:
	rm -f $(ALL) lint.out tags
