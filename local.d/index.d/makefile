#
#	$Header: /home/Vince/cvs/local.d/index.d/makefile,v 1.3 1990-03-30 17:02:55 vrs Exp $
#
ALL	= index
include /etc/CONFIG

install: $(ALL) $(BINDIR)
	cp $(ALL) $(BINDIR)
	cd $(BINDIR); chmod 711 $(ALL)

clean:
	rm -f $(ALL) index.o

index:	index.o
	$(CC) $(CFLAGS) $(LDFLAGS) $? -ll -o $@
