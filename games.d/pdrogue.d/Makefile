#
CFLAGS	= $(CXENIX)
LFLAGS	= $(LXENIX)
SCOREF	= /usr/games/lib/pdrogue_roll
#
SRCS	= door_course.c hit.c init.c inventory.c level.c main.c message.c \
	  monster.c move.c object.c pack.c play.c random.c room.c score.c \
	  special_hit.c throw.c use.c zap.c
OBJS	= $(SRCS:.c=.o)
CXENIX	= -O -i -DSCOREFILE=\"$(SCOREF)\" -Drindex=strrchr -Dsrandom=srand
LXENIX	= -lcurses -ltermcap -lx

a.out:	$(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS) $(LFLAGS)
