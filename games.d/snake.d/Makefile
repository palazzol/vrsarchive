#
#	snake %W% %G%
#
#
ALL	= snake snake2
OPTIONS	= -O
MODEL	= -i -F 2000
include /etc/CONFIG

snake:	snake.c
	$(CC) $(CFLAGS) -o $@ snake.c $(LIBS)

snake2: snake2.c
	$(CC) $(CFLAGS) -o $@ snake2.c $(LIBS)

lint:
	lint -lcurses -ltermlib snake.c | tee lint.out

clean:
	rm -f $(ALL) core

install: snake snake2 $(GAMDIR)
	cp snake $(GAMDIR)
	chmod 711 $(GAMDIR)/snake
	cp snake2 $(GAMDIR)
	chmod 711 $(GAMDIR)/snake2
