#
#	$Header: /home/Vince/cvs/local.d/index.d/makefile,v 1.2 1987-02-15 15:53:47 vrs Exp $
#
ALL	= index
include ../../CONFIG

install: $(ALL) $(BINDIR)
	cp $(ALL) $(BINDIR)
	cd $(BINDIR); chmod 711 $(ALL)

clean:
	rm -f $(ALL) index.o

index:	index.o
	$(CC) $(CFLAGS) $(LDFLAGS) $? -ll -o $@
