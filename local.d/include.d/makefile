#
#	$Header: /home/Vince/cvs/local.d/include.d/makefile,v 1.3 1990-03-30 17:02:54 vrs Exp $
#
ALL	= include
MODEL	= -i
include /etc/CONFIG

install: $(ALL) $(BINDIR)
	cp $(ALL) $(BINDIR)
	cd $(BINDIR); chmod 711 $(ALL)

lint:
	lint $(CFLAGS) include.c | tee lint.out

clean:
	rm -f $(ALL)
