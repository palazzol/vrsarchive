#
#	$Header: /home/Vince/cvs/local.d/displays.d/makefile,v 1.2 1987-07-17 15:48:58 vrs Exp $
#
ALL	= window
OPTIONS	= -O
MODEL	= -i -F 2000
include ../../CONFIG
SRCS	= keyboard.c main.c pdisplay.c pipe.c pty.c vdisplay.c
OBJS	= $(SRCS:.c=.o)

window: $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $@ $(LIBS)

clean:
	rm -f lint window $(OBJS)

install: $(ALL) $(BINDIR)
	cp $(ALL) $(BINDIR)
	cd $(BINDIR); chown root $(ALL)
	cd $(BINDIR); chmod 4711 $(ALL)

lint:	$(SRCS)
	lint $(OPTIONS) $(LIBS) $(SRCS) | tee $@

$(OBJS): globals.h
