#
# Simple makefile for rot program
#
ALL	= rot
MODEL	= -i -F 2000
OPTIONS	= -O
include ../../CONFIG

install: $(ALL)
	cp $(ALL) $(GAMDIR)
	cd $(GAMDIR); chmod 711 $(ALL)

clean:
	rm -f $(ALL)

test:	rot
	rot <rot.c
