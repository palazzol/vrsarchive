#
#	$Header: /home/Vince/cvs/local.d/umodem.d/Makefile,v 1.3 2002-11-23 07:51:58 Vincent Exp $
#
ALL	= umodem
MODEL	= -i
OPTIONS	= -O
include $(HOME)/CONFIG

install: $(ALL) $(MAN1) $(BINDIR)
	cp $(ALL) $(BINDIR)
	cd $(BINDIR); chmod 711 $(ALL)

clean:
	rm -f $(ALL)
