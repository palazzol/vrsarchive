#
#	$Header: /home/Vince/cvs/local.d/tpr.d/makefile,v 1.2 1987-02-15 16:00:02 vrs Exp $
#
ALL	= tpr
MODEL	= -i
OPTIONS	= -O
include ../../CONFIG
OBJS	= tpr0.o tpr1.o itoeng.o

tpr:	$(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS)

install: $(ALL) $(BINDIR) $(MAN1)
	cp $(ALL) $(BINDIR)
	cd $(BINDIR); chmod 711 $(ALL)
	cp tpr.1 $(MAN1)
	cd $(MAN1); chmod 644 tpr.1

lint:
	lint $(OBJS:.o=.c) | tee lint.out

clean:
	rm -f $(OBJS) $(ALL)
