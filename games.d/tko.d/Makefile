ALL	= tko
OPTIONS	= -O -Drindex=strrchr
MODEL	= -i
include ../../CONFIG

tko:	tko.o names.o subs1.o subs2.o tables.o
	cc $(CFLAGS) tko.o names.o subs1.o subs2.o tables.o -o tko

tko.o: def.h extern.h tko.c
	cc -c $(CFLAGS) tko.c
names.o: def.h extern.h names.c
	cc -c $(CFLAGS) names.c
subs1.o: def.h extern.h subs1.c
	cc -c $(CFLAGS) subs1.c
subs2.o: def.h extern.h subs2.c
	cc -c $(CFLAGS) subs2.c
tables.o: def.h extern.h tables.c
	cc -c $(CFLAGS) tables.c

install: tko $(GAMDIR)
	cp tko $(GAMDIR)
	chmod 711 $(GAMDIR)/tko

clean:
	rm -f tko.o names.o subs1.o subs2.o tables.o tko make.out core
