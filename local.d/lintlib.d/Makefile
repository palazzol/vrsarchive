#
#	$Header: /home/Vince/cvs/local.d/lintlib.d/Makefile,v 1.5 2002-11-23 07:51:53 Vincent Exp $
#
OBJECTS	= llibcurses.ln
SRCS	= $(OBJECTS:.ln=)
ALL	= $(OBJECTS) $(SRCS)
include $(HOME)/CONFIG
.SUFFIXES: .ln

.c.ln:
	$(LINT) -c -n $?
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
