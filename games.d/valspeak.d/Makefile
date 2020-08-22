#
# Makefile for jive and valspeak programs
#
ALL	= jive$(EXE) valspeak$(EXE)
MODEL	= -i
OPTIONS	= -O
include $(HOME)/CONFIG

.l$(EXE):
	$(LEX) $?
	$(CC) $(CFLAGS) lex.yy.c -o $@ $(LIBS) $(LEXLIB)
	rm -f lex.yy.c lex.yy.o

clean:
	rm -f $(ALL)

install: $(ALL)
	cp $(ALL) $(GAMDIR)
	cd $(GAMDIR); chmod 711 $(ALL)
