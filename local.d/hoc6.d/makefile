#
#	$Header: /home/Vince/cvs/local.d/hoc6.d/makefile,v 1.3 1990-03-30 17:02:52 vrs Exp $
#
ALL	= hoc6
MODEL	= -i
include /etc/CONFIG
YFLAGS	= -d
OBJS	= hoc.o code.o init.o math.o symbol.o

hoc6:  $(OBJS)
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
