#
#	$Header: /home/Vince/cvs/local.d/compress.d/makefile,v 1.3 1990-03-30 17:02:30 vrs Exp $
#
MODEL	= -i -F 3000
OPTIONS	= -O -DSHORT_INT
ALL	= compress
include /etc/CONFIG

compress.o: compress.c USERMEM
	cc -c $(CFLAGS) compress.c

# USERMEM may have to be set by hand.  It should contain the amount of
# available user memory in bytes.  Set it to zero for physical memory
# less than 1 Meg.
USERMEM:
	sh usermem > USERMEM

install: $(ALL) $(BINDIR) $(MAN1)
	cp $(ALL) $(BINDIR)
	cd $(BINDIR); chmod 711 $(ALL)
	rm -f $(BINDIR)/uncompress $(BINDIR)/zcat
	ln $(BINDIR)/compress $(BINDIR)/uncompress
	ln $(BINDIR)/compress $(BINDIR)/zcat
#	cp compress.1 $(MAN1)
#	cd $(MAN1); chmod 644 compress.1

clean:
	rm -f $(ALL) uncompress zcat core
