SHELL	= /bin/sh
SRCS	= ditk.c ditk.lib.c
OBJS	= $(SRCS:.c=.o)
CFLAGS	= -Mm -O
LIBS	= -lm

ditk:	$(OBJS)
	$(CC) $(CFLAGS) $(OBJS) $(LIBS) -o $@

lint:	$(SRCS)
	lint $(SRCS) | tee $@

clean:
	rm -f lint ditk $(OBJS)
