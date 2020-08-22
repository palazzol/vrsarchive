#
#	$Header: /home/Vince/cvs/local.d/ofiles.d/Makefile,v 1.6 2002-11-24 17:36:17 vrs Exp $
#	Ofiles %W%, %G% cspencer
#
ALL	= ofiles$(EXE)
MODEL	= -Me
OPTIONS	= -O
include $(HOME)/CONFIG
OBJS	= ofiles.o fstab.o

all: ofiles$(EXE)

ofiles$(EXE):	$(OBJS)
	$(CC) $(CFLAGS) $(OBJS) $(LIBS) -o $@

install: $(ALL) $(BINDIR) $(MAN8)
	cp $(ALL) $(BINDIR)
	cd $(BINDIR); chown root $(ALL)
	cd $(BINDIR); chgrp root $(ALL)
	cd $(BINDIR); chmod 4711 $(ALL)
	cp ofiles.8 $(MAN8)
	cd $(MAN8); chmod 644 ofiles.8

clean:
	rm -f $(ALL) $(OBJS)
