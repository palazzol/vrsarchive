OPTIONS	=
MODEL	= -i -F 2000
CFLAGS	= $(OPTIONS) $(MODEL)
LIBS	= -lcurses -ltermcap -lx
SRCS	= keyboard.c main.c pdisplay.c pty.c vdisplay.c
OBJS	= $(SRCS:.c=.o)

window: $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $@ $(LIBS)

clean:
	rm -f lint window $(OBJS)

lint:	$(SRCS)
	lint $(OPTIONS) $(LIBS) $(SRCS) | tee $@

$(OBJS): globals.h
