#
#	$Header: /home/Vince/cvs/local.d/index.d/makefile,v 1.4 2002-11-23 07:49:08 Vincent Exp $
#
ALL	= index
include $(HOME)/CONFIG

install: $(ALL) $(BINDIR)
	cp $(ALL) $(BINDIR)
	cd $(BINDIR); chmod 711 $(ALL)

clean:
	rm -f $(ALL) index.o

index:	index.o
	$(CC) $(CFLAGS) $(LDFLAGS) $? $(LEXLIB) -o $@
