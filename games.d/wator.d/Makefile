#
#	$Header: /home/Vince/cvs/games.d/wator.d/Makefile,v 1.4 2002-11-23 04:09:49 Vincent Exp $
#
ALL	= wator$(EXE)
OPTIONS	= -O
MODEL	= -i -F 2000
include $(HOME)/CONFIG
OBJS	= wator.o screen_ctl.o item_control.o init.o parse_args.o \
	  fish_cycle.o shark_cycle.o

wator$(EXE):	$(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS) $(LIBS)

wator.o: wator.h
screen_ctl.o: wator.h
item_control.o: wator.h
init.o: wator.h
parse_args.o: wator.h
fish_cycle.o: wator.h
shark_cycle.o: wator.h

clean:
	rm -f $(ALL) $(OBJS)

install: $(ALL) $(GAMDIR) $(MAN6)
	cp $(ALL) $(GAMDIR)
	cd $(GAMDIR); chmod 711 $(ALL)
	cp wator.6 $(MAN6)
	cd $(MAN6); chmod 644 wator.6
