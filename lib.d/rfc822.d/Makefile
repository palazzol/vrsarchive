OPTIONS	= -O
ALL		= atest
include /etc/CONFIG
OBJS	= addr.o rfc822.o
SRCS	= addr.c rfc822.c atest.c

atest:		$(OBJS) atest.c
	$(CC) -o atest $(CFLAGS) atest.c $(OBJS)

rfc822.o:	rfc822.c
$(OBJS):	addr.h

clean:
	rm -f a.out foo core tags lint rfc822.c atest *.o y.*

lint:		all
	exec lint -hba $(SRCS) >lint 
