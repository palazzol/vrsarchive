#
#	$Header: /home/Vince/cvs/local.d/lintlib.d/Makefile,v 1.1 1987-04-18 21:55:49 vrs Exp $
#
ALL	= llibcurses.ln
include ../../CONFIG

llibcurses.ln: llibcurses
	/lib/cpp -C -Dlint $? | /usr/lib/lint1 -H/tmp/$$ -v > $@

install: $(ALL)
	cp $(ALL) $(LIBDIR)
	cd $(LIBDIR); chmod 644 $(ALL)

clean:
	rm -f $(ALL)
