#
CFLAGS	= $(CXENIX)
LFLAGS	= $(LXENIX)
GAME	= pdrogue
GAMDIR	= $(DESTDIR)/usr/games
GAMLIB	= $(GAMDIR)/lib
SCOREF	= $(GAMLIB)/$(GAME)_roll
#
SHELL	= /bin/sh
SRCS	= door_c.c hit.c init.c inventory.c level.c main.c message.c \
	  monster.c move.c object.c pack.c play.c random.c room.c score.c \
	  special_h.c throw.c use.c zap.c
OBJS	= $(SRCS:.c=.o)
CXENIX	= -O -i -DSCOREFILE=\"$(SCOREF)\" -Drindex=strrchr -Dsrandom=srand
LXENIX	= -lcurses -ltermcap -lx

a.out:	$(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS) $(LFLAGS)

$(GAME): a.out
	cp $? $@
	strip $@

$(GAMDIR) $(GAMLIB):
	mkdir $@
	chmod 755 $@

install: $(GAME) $(GAMDIR) $(GAMLIB)
	cp $(GAME) $(GAMDIR)
	chown cron $(GAMDIR)/$(GAME)
	chmod 4711 $(GAMDIR)/$(GAME)
	touch $(SCOREF)
	chown cron $(SCOREF)
	chmod u+w $(SCOREF)

clean:
	rm -f $(OBJS) a.out $(GAME) make.out core
