#
#	$Header: /home/Vince/cvs/local.d/lintlib.d/Makefile,v 1.4 1990-04-13 14:33:35 vrs Exp $
#
OBJECTS	= llibcurses.ln
SRCS	= $(OBJECTS:.ln=)
ALL	= $(OBJECTS) $(SRCS)
include /etc/CONFIG
.SUFFIXES: .ln

.c.ln:
	lint -c -n $?
# If your lint doesn't have -c, maybe this will work:
#	/lib/cpp -C -Dlint $? | /usr/lib/lint1 -H/tmp/$$ -v > $@+
#	mv $@+ $@
.c:
	cp $? $@

install: $(ALL)
	cp $(ALL) $(LIBDIR)
	cd $(LIBDIR); chmod 644 $(ALL)

clean:
	rm -f $(ALL)
