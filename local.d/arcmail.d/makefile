#
#	$Header: /home/Vince/cvs/local.d/arcmail.d/makefile,v 1.2 1987-02-15 15:48:47 vrs Exp $
#
ALL	= arcmail
MODEL	= -i -F 2000
include ../../CONFIG

install: $(ALL) $(BINDIR)
	cp $(ALL) $(BINDIR)
	cd $(BINDIR); chmod 711 $(ALL)

lint:
	lint arcmail.c | tee lint.out

clean:
	rm -f $(ALL)
