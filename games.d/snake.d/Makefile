#
#	snake %W% %G%
#
#
ALL	= snake$(EXE) snake2$(EXE)
OPTIONS	= -O
MODEL	= -i -F 2000
include $(HOME)/CONFIG

snake$(EXE):	snake.c
	$(CC) $(CFLAGS) -o $@ snake.c $(LIBS)

snake2$(EXE): snake2.c
	$(CC) $(CFLAGS) -o $@ snake2.c $(LIBS)

lint:
	lint -lcurses -ltermlib snake.c | tee lint.out

clean:
	rm -f $(ALL) core

install: snake$(EXE) snake2$(EXE) $(GAMDIR)
	cp snake$(EXE) $(GAMDIR)
	chmod 711 $(GAMDIR)/snake$(EXE)
	cp snake2$(EXE) $(GAMDIR)
	chmod 711 $(GAMDIR)/snake2$(EXE)
