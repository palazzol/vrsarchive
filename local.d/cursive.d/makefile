MODEL	= -i
OPTIONS	= -O
ALL	= cursive$(EXE)
include $(HOME)/CONFIG
OBJ	= cursive.o font.o

cursive$(EXE): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $(OBJ) 

install: $(ALL) $(BINDIR) $(MAN1)
	cp $(ALL) $(BINDIR)
	cd $(BINDIR); chmod 711 $(ALL)
	cp cursive.6 $(MAN1)/cursive.1
	chmod 644 $(MAN1)/cursive.1

clean:
	rm -f $(ALL) $(OBJ)
