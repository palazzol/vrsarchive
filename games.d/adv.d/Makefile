#
#	$Header: /home/Vince/cvs/games.d/adv.d/Makefile,v 1.3 1990-03-27 17:24:03 vrs Exp $
#
MODEL	= -i -F 2000
OPTIONS	= -O -DCURSED # Use -DCURSED to get the "curses" version
ALL	= adv aland pirate
include /etc/CONFIG
OBJS	= main.o alc.o str.o readin.o nxtwrd.o rdr.o init.o\
	  play.o misc.o prt.o file.o disply.o
SRCS	= main.c alc.c str.c readin.c nxtwrd.c rdr.c init.c\
	  play.c misc.c prt.c file.c disply.c

$(OBJS): adv.h

adv:	$(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS) $(LIBS)

aland:	../../CONFIG
	echo exec $(GAMLIB)/adv $(GAMLIB)/al > $@

pirate:	../../CONFIG
	echo exec $(GAMLIB)/adv $(GAMLIB)/p > $@

lint:
	lint -h adv.h $(SRCS)
	
list:
	llist -t adv DOC adv.h $(SRCS)

clean:
	rm -f $(OBJS) $(ALL)

install: $(ALL) $(GAMDIR) $(GAMLIB)
	cp adv al p $(GAMLIB)
	cd $(GAMLIB); chmod 711 adv
	cd $(GAMLIB); chmod 644 al p
	cp aland pirate $(GAMDIR)
	cd $(GAMDIR); chmod 755 aland pirate
