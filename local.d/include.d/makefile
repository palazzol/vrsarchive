#
#	$Header: /home/Vince/cvs/local.d/include.d/makefile,v 1.4 2002-11-23 07:49:07 Vincent Exp $
#
ALL	= include
MODEL	= -i
include $(HOME)/CONFIG

install: $(ALL) $(BINDIR)
	cp $(ALL) $(BINDIR)
	cd $(BINDIR); chmod 711 $(ALL)

lint:
	lint $(CFLAGS) include.c | tee lint.out

clean:
	rm -f $(ALL)
