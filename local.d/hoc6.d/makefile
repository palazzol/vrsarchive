#
#	$Header: /home/Vince/cvs/local.d/hoc6.d/makefile,v 1.5 2002-11-24 05:58:46 Vincent Exp $
#
ALL	= hoc6$(EXE)
MODEL	= -i
include $(HOME)/CONFIG
YFLAGS	= -d
OBJS	= hoc.o code.o init.o math.o symbol.o

hoc6$(EXE):  $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) $(LIBS) -o $@

install: $(ALL) $(BINDIR)
	cp $(ALL) $(BINDIR)
	cd $(BINDIR); chmod 711 $(ALL)

clean:
	rm -f $(OBJS) [xy].tab.[ch] $(ALL)

hoc.o code.o init.o symbol.o: hoc.h

code.o init.o symbol.o: x.tab.h

x.tab.h: y.tab.h
	-cmp -s x.tab.h y.tab.h || cp y.tab.h x.tab.h

pr: hoc.y hoc.h code.c init.c math.c symbol.c
	@pr $?
	@touch pr
