#
# Makefile for jive and valspeak programs
#
ALL	= jive valspeak
MODEL	= -i
OPTIONS	= -O
include ../../CONFIG

.l:
	lex $?
	$(CC) $(CFLAGS) lex.yy.c -o $@ $(LIBS) -ll
	rm -f lex.yy.c lex.yy.o

clean:
	rm -f $(ALL)

install: $(ALL)
	cp $(ALL) $(GAMDIR)
	cd $(GAMDIR); chmod 711 $(ALL)
