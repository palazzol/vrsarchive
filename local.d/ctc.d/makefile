ALL	= ctc
MODEL	= -i -F 2000
OPTIONS	= -O
include $(HOME)/CONFIG
OBJ	= ctc.o termcap.o

install: $(ALL) $(BINDIR) $(MAN1)
	cp $(ALL) $(BINDIR)
	cd $(BINDIR); chmod 711 $(ALL)
	cp ctc.1 $(MAN1)
	cd $(MAN1); chmod 644 ctc.1

ctc:    $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o $@ $(LIBS)

clean:
	rm -f $(OBJ) $(ALL)
