#
#	Makefile for gothic banner print program.
ALL	= gothic
include /etc/CONFIG
OBJ =		gothic0.o gothic1.o

gothic:	$(OBJ)
		$(CC) $(CFLAGS) -o $@ $(OBJ)

clean:
		rm -f $(ALL) $(OBJ)

install: $(ALL)
		cp $(ALL) $(GAMDIR)
		cd $(GAMDIR); chmod 711 $(ALL)
