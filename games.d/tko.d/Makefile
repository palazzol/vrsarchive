ALL	= tko
OPTIONS	= -O -Drindex=strrchr
MODEL	= -i
include ../../CONFIG
OBJS	= tko.o names.o subs1.o subs2.o tables.o

tko:	$(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $@ $(LIBS)

install: $(ALL) $(GAMDIR)
	cp $(ALL) $(GAMDIR)
	cd $(GAMDIR); chmod 711 $(ALL)

clean:
	rm -f $(OBJS) $(ALL) make.out core

tko.o: def.h extern.h tko.c
names.o: def.h extern.h names.c
subs1.o: def.h extern.h subs1.c
subs2.o: def.h extern.h subs2.c
tables.o: def.h extern.h tables.c
