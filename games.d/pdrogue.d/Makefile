#
#	$Header: /home/Vince/cvs/games.d/pdrogue.d/Makefile,v 1.4 1987-06-08 22:03:26 vrs Exp $
#
ALL	= pdrogue
SCOREF	= $(GAMLIB)/pdrogue_roll
MODEL	= -i
OPTIONS	= -DSCOREFILE=\"$(SCOREF)\" -Dsrandom=srand -O
include ../../CONFIG
SRCS	= door_c.c hit.c init.c inventory.c level.c main.c message.c \
	  monster.c move.c object.c pack.c play.c random.c room.c score.c \
	  special_h.c throw.c use.c zap.c
OBJS	= $(SRCS:.c=.o)

pdrogue: $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS) $(LIBS)

install: $(ALL) $(GAMDIR) $(GAMLIB)
	cp $(ALL) $(GAMDIR)
	cd $(GAMDIR); chown $(SAFEUID) $(ALL)
	cd $(GAMDIR); chmod 4711 $(ALL)
	touch $(SCOREF)
	chown $(SAFEUID) $(SCOREF)
	chmod 644 $(SCOREF)

clean:
	rm -f $(OBJS) $(ALL) make.out core
