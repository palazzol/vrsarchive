CFLAGS = -dos -O
tko.exe: tko.o names.o subs1.o subs2.o tables.o
	cc $(CFLAGS) tko.o names.o subs1.o subs2.o tables.o -o tko.exe
	chmod 0700 tko.exe
tko.o: def.h extern.h tko.c
	cc -c $(CFLAGS) tko.c
names.o: def.h extern.h names.c
	cc -c $(CFLAGS) names.c
subs1.o: def.h extern.h subs1.c
	cc -c -Drindex=strrchr $(CFLAGS) subs1.c
subs2.o: def.h extern.h subs2.c
	cc -c $(CFLAGS) subs2.c
tables.o: def.h extern.h tables.c
	cc -c $(CFLAGS) tables.c
