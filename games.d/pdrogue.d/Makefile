#
#	$Header: /home/Vince/cvs/games.d/pdrogue.d/Makefile,v 1.7 1990-03-27 17:26:40 vrs Exp $
#
ALL	= pdrogue
SCOREF	= $(GAMLIB)/pdrogue_roll
MODEL	= -M2m -F 2000
OPTIONS	= -DUNIX -DSCORE=\"$(SCOREF)\" -O
include /etc/CONFIG
OBJS	= curses.o hit.o init.o inventory.o level.o machdep.o main.o \
	  message.o monster.o move.o object.o pack.o play.o random.o ring.o \
	  room.o save.o score.o spec_hit.o throw.o trap.o use.o zap.o
SRCS	= $(OBJS:.o=.c)

pdrogue: $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS) $(LIBS)

install: $(ALL) $(GAMDIR) $(GAMLIB) $(MAN6)
	cp $(ALL) $(GAMDIR)/
	cd $(GAMDIR); chown $(SAFEUID) $(ALL)
	cd $(GAMDIR); chmod 4711 $(ALL)
	touch $(SCOREF)
	chown $(SAFEUID) $(SCOREF)
	chmod 644 $(SCOREF)
	cp rogue.6 $(MAN6)/pdrogue.6
	cd $(MAN6); chmod 644 pdrogue.6

clean:
	rm -f $(OBJS) $(ALL) make.out core

curses.o: rogue.h
hit.o: rogue.h
init.o: rogue.h
inventory.o: rogue.h
level.o: rogue.h
machdep.o: rogue.h
main.o: rogue.h
message.o: rogue.h
monster.o: rogue.h
move.o: rogue.h
object.o: rogue.h
pack.o: rogue.h
play.o: rogue.h
ring.o: rogue.h
room.o: rogue.h
save.o: rogue.h
score.o: rogue.h
spec_hit.o: rogue.h
throw.o: rogue.h
trap.o: rogue.h
use.o: rogue.h
zap.o: rogue.h
