# BINDIR is the directory where the moria binary while be put
ALL	= moria$(EXE)
OPTIONS	= -DUSG -DKEY_BINDINGS=ROGUE_LIKE -DLIBDIR=$(LIBDIR)
MODEL	= -M2lt20 -F 8000
include $(HOME)/CONFIG
LIBDIR = $(GAMLIB)/moriadir

SRCS	= main.c misc1.c misc2.c store1.c files.c io.c create.c desc.c\
	  generate.c sets.c dungeon.c creature.c death.c eat.c help.c magic.c\
	  potions.c prayer.c save.c staffs.c wands.c scrolls.c spells.c\
	  wizard.c store2.c signals.c moria1.c moria2.c monsters.c\
	  treasure1.c treasure2.c variables.c var2.c

OBJS	= $(SRCS:.c=.o)

moria$(EXE) : $(OBJS)
	$(CC) -o $@ $(CFLAGS) $(OBJS) $(LIBS)

clean:
	rm -f $(ALL) $(OBJS) x.c xs.c tmp.c tmp.o

lintout : $(SRCS)
	lint -DUSG $(SRCS) -lm -lcurses -ltermcap > lintout

TAGS : $(SRCS)
	ctags -x $(SRCS) > TAGS

$(LIBDIR):
	mkdir $@
	chmod 755 $@
	chown $(SAFEUID) $@

install: $(ALL) $(GAMDIR) $(GAMLIB) $(LIBDIR)
	cp moria$(EXE) $(GAMDIR)
	cp Moria_hours $(LIBDIR)
	cp Moria_news $(LIBDIR)
	cp Highscores $(LIBDIR)
	chown $(SAFEUID) $(GAMDIR)/moria$(EXE)
	chmod 4511 $(GAMDIR)/moria$(EXE)
	chmod 644 $(LIBDIR)/Highscores
	chmod 444 $(LIBDIR)/Moria_news
	chmod 444 $(LIBDIR)/Moria_hours
	chmod 555 $(LIBDIR)
	chown $(SAFEUID) $(LIBDIR) $(LIBDIR)/*

create.o: constants.h types.h externs.h
creature.o: constants.h types.h externs.h
death.o: config.h constants.h types.h externs.h
desc.o: constants.h types.h externs.h
dungeon.o: constants.h types.h externs.h
eat.o: constants.h types.h externs.h
files.o: config.h constants.h types.h externs.h
generate.o: constants.h types.h externs.h
help.o: constants.h types.h externs.h
io.o: constants.h types.h externs.h
magic.o: constants.h types.h externs.h
main.o: constants.h types.h 
misc1.o: constants.h types.h externs.h
misc2.o: config.h constants.h types.h externs.h
moria1.o: constants.h types.h externs.h
moria2.o: constants.h types.h externs.h
potions.o: constants.h types.h externs.h
prayer.o: constants.h types.h externs.h
save.o: constants.h types.h externs.h
scrolls.o: constants.h types.h externs.h
sets.o: constants.h
signals.o: constants.h types.h externs.h
spells.o: constants.h types.h externs.h
staffs.o: constants.h types.h externs.h
store1.o: constants.h types.h externs.h
store2.o: constants.h types.h externs.h
wands.o: constants.h types.h externs.h
wizard.o: constants.h types.h externs.h
