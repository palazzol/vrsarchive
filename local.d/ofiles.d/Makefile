#
#	$Header: /home/Vince/cvs/local.d/ofiles.d/Makefile,v 1.2 1987-02-15 15:57:49 vrs Exp $
#	Ofiles %W%, %G% cspencer
#
ALL	= ofiles
MODEL	= -Me
OPTIONS	= -O
include ../../CONFIG
OBJS	= ofiles.o fstab.o

all: ofiles

ofiles:	$(OBJS)
	$(CC) $(CFLAGS) $(OBJS) $(LIBS) -o $@

install: $(ALL)
	cp $(ALL) $(BINDIR)
	cd $(BINDIR); chown root $(ALL)
	cd $(BINDIR); chgrp root $(ALL)
	cd $(BINDIR); chmod 4711 $(ALL)
	cp ofiles.8 $(MAN8)
	cd $(MAN8); chmod 644 ofiles.8

clean:
	rm -f $(ALL) $(OBJS)
