#
# Simple makefile for rot program
#
ALL	= rot$(EXE)
MODEL	= -i -F 2000
OPTIONS	= -O
include $(HOME)/CONFIG

install: $(ALL)
	cp $(ALL) $(GAMDIR)
	cd $(GAMDIR); chmod 711 $(ALL)
	cp rot.1 $(MAN6)
	cd $(MAN6); chmod 644 rot.1

clean:
	rm -f $(ALL)

test:	rot
	rot <rot.c
