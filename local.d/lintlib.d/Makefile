#
#	$Header: /home/Vince/cvs/local.d/lintlib.d/Makefile,v 1.2 1987-04-18 21:59:37 vrs Exp $
#
OBJECTS	= llibcurses.ln
SRCS	= $(OBJECTS:.ln=)
ALL	= $(OBJECTS) $(SRCS)
include ../../CONFIG

llibcurses.ln: llibcurses
	/lib/cpp -C -Dlint $? | /usr/lib/lint1 -H/tmp/$$ -v > $@

install: $(ALL)
	cp $(ALL) $(LIBDIR)
	cd $(LIBDIR); chmod 644 $(ALL)

clean:
	rm -f $(OBJECTS)
