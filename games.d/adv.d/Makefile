#
#	$Header: /home/Vince/cvs/games.d/adv.d/Makefile,v 1.5 2002-11-23 04:09:34 Vincent Exp $
#
MODEL	= -i -F 2000
# Use -DCURSED to get the "curses" version
OPTIONS	= -O -DCURSED
ALL	= adv$(EXE) aland pirate
CONFIG	= $(HOME)/CONFIG
include $(CONFIG)
OBJS	= main.o alc.o str.o readin.o nxtwrd.o rdr.o init.o\
	  play.o misc.o prt.o file.o disply.o
SRCS	= main.c alc.c str.c readin.c nxtwrd.c rdr.c init.c\
	  play.c misc.c prt.c file.c disply.c

$(OBJS): adv.h

adv$(EXE):	$(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS) $(LIBS)

aland:	$(CONFIG)
	echo exec $(GAMLIB)/adv $(GAMLIB)/al > $@

pirate:	$(CONFIG)
	echo exec $(GAMLIB)/adv $(GAMLIB)/p > $@

lint:
	lint -h adv.h $(SRCS)
	
list:
	llist -t adv DOC adv.h $(SRCS)

clean:
	rm -f $(OBJS) $(ALL)

install: $(ALL) $(GAMDIR) $(GAMLIB)
	cp adv$(EXE) al p $(GAMLIB)
	cd $(GAMLIB); chmod 711 adv$(EXE)
	cd $(GAMLIB); chmod 644 al p
	cp aland pirate $(GAMDIR)
	cd $(GAMDIR); chmod 755 aland pirate
