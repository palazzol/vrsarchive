#
#	$Header: /home/Vince/cvs/local.d/tpr.d/makefile,v 1.3 1990-03-30 17:03:40 vrs Exp $
#
ALL	= tpr
MODEL	= -i
OPTIONS	= -O
include /etc/CONFIG
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
