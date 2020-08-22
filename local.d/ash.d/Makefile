#
#	$Header: /home/Vince/cvs/local.d/ash.d/Makefile,v 1.6 2002-11-26 07:35:59 vrs Exp $
#
ALL	= ash ash.has
OPTIONS	= -O
MODEL	= -i
include $(HOME)/CONFIG

install: $(ALL) $(BINDIR) $(LIBDIR) $(MAN1)
	cp ash $(BINDIR)
	cd $(BINDIR); chmod 755 ash
	cp ash.help ash.inst ash.has $(LIBDIR)
	cd $(LIBDIR); chmod 644 ash.help ash.inst
	cd $(LIBDIR); chmod 711 ash.has
	cp ash.1 $(MAN1)/
	cd $(MAN1); chmod 644 ash.1
#
#	Special rule does fixups needed for current system
#
ash:	ash.sh
	echo "#!/bin/sh" >>$@+
	echo HELP=$(LIBDIR)/ash.help >>$@+
	echo INST=$(LIBDIR)/ash.inst >>$@+
	echo HAS=$(LIBDIR)/ash.has >>$@+
	echo PAGE=$(PAGER) >>$@+
	echo 'E2="\c"' >>$@+
	echo 'FMT="pr -t -5"' >> $@+
	cat $< >>$@+
	mv $@+ $@

ash.has: has.c
	$(CC) $(CFLAGS) has.c -o $@
	rm -f has.o
clean:
	rm -f $(ALL)
