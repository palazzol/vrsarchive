#
#	%W%	%G%
#
OPTIONS	= -O
MODEL	= -i
ALL	= solve
include ../../CONFIG
OBJS	= solve.o
CFILES	= solve.c

solve: $(OBJS)
	$(CC) -o $@ $(OBJS) $(LIBS)

tags:	$(CFILES)
	ctags $(CFILES)

install: $(ALL) $(GAMDIR) $(MAN6)
	cp $(ALL) $(GAMDIR)
	cd $(GAMDIR); chmod 711 $(ALL)
	cp solve.6 $(MAN6)/solve.6
	chmod 644 $(MAN6)/solve.6

clean:
	rm -f $(OBJS) $(ALL) core make.out
