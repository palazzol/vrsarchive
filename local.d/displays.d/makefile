#
#	$Header: /home/Vince/cvs/local.d/displays.d/makefile,v 1.5 2002-11-23 07:49:05 Vincent Exp $
#
ALL	= displays
OPTIONS	= -O
MODEL	= -i -F 2000
include $(HOME)/CONFIG
SRCS	= keyboard.c main.c pdisplay.c pipe.c pty.c vdisplay.c
OBJS	= $(SRCS:.c=.o)

$(ALL): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $@ $(LIBS)

clean:
	rm -f lint $(ALL) $(OBJS)

install: $(ALL) $(BINDIR)
	cp $(ALL) $(BINDIR)
	cd $(BINDIR); chown root $(ALL)
	cd $(BINDIR); chmod 4711 $(ALL)

lint:	$(SRCS)
	lint $(OPTIONS) $(LIBS) $(SRCS) | tee $@

$(OBJS): globals.h
