#
#	Makefile for gothic banner print program.
ALL	= gothic$(EXE)
include $(HOME)/CONFIG
OBJ =		gothic0.o gothic1.o

gothic$(EXE):	$(OBJ)
		$(CC) $(CFLAGS) -o $@ $(OBJ)

clean:
		rm -f $(ALL) $(OBJ)

install: $(ALL)
		cp $(ALL) $(GAMDIR)
		cd $(GAMDIR); chmod 711 $(ALL)
