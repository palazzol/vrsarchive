ALL	= dithds
OPTIONS	= -O
MODEL	= -i
include $(HOME)/CONFIG
SRCS	= dithds.c dithds.lib.c
OBJS	= $(SRCS:.c=.o)

dithds:	$(OBJS)
	$(CC) $(CFLAGS) $(OBJS) $(LIBS) -o $@

lint:	$(SRCS)
	lint $(SRCS) | tee $@

install: $(ALL) $(BINDIR) $(MAN1)
	cp $(ALL) $(BINDIR)
	cd $(BINDIR); chmod 711 $(ALL)
	cp dithds.1 $(MAN1)
	cd $(MAN1); chmod 644 dithds.1

clean:
	rm -f lint $(ALL) $(OBJS)
