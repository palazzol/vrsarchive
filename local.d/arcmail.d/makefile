#
#	$Header: /home/Vince/cvs/local.d/arcmail.d/makefile,v 1.3 1990-03-30 17:02:16 vrs Exp $
#
ALL	= arcmail
MODEL	= -i -F 2000
include /etc/CONFIG

install: $(ALL) $(BINDIR)
	cp $(ALL) $(BINDIR)
	cd $(BINDIR); chmod 711 $(ALL)

lint:
	lint arcmail.c | tee lint.out

clean:
	rm -f $(ALL)
