#
#	$Header: /home/Vince/cvs/local.d/lintlib.d/Makefile,v 1.3 1990-03-30 17:03:02 vrs Exp $
#
OBJECTS	= llibcurses.ln
SRCS	= $(OBJECTS:.ln=)
ALL	= $(OBJECTS) $(SRCS)
include /etc/CONFIG

llibcurses.ln: llibcurses
	/lib/cpp -C -Dlint $? | /usr/lib/lint1 -H/tmp/$$ -v > $@

install: $(ALL)
	cp $(ALL) $(LIBDIR)
	cd $(LIBDIR); chmod 644 $(ALL)

clean:
	rm -f $(OBJECTS)
