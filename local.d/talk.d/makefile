#
#	$Header: /home/Vince/cvs/local.d/talk.d/makefile,v 1.6 2002-11-23 07:51:57 Vincent Exp $
#
ALL	= talk
MODEL	= -i -F 2000
OPTIONS	= -O
include $(HOME)/CONFIG
OBJECT	= talk.o announce.o ctl.o display.o get_names.o init_disp.o invite.o \
	  io.o msgs.o
SOURCE	= talk.c announce.c ctl.c display.c get_names.c init_disp.c invite.c \
	  io.c msgs.c
INCLUDE	= talk.h

talk:	$(OBJECT)
	$(CC) $(CFLAGS) -o $@ $(OBJECT) $(LIBS)

install: $(ALL) $(BINDIR)
	cp $(ALL) $(BINDIR)
	cd $(BINDIR); chmod 711 $(ALL)

$(OBJECT): $(INCLUDE)

lint:
	lint -lcurses -ltermlib $(SOURCE) | tee lint.out

clean:
	rm -f $(OBJECT) $(ALL) lint.out tags

tags:	$(SOURCE) $(INCLUDE)
	rm -f tags
	ctags $(SOURCE) $(INCLUDE)
