#
# include %W%	%G%
#
#
SHELL	= /bin/sh
CFLAGS	= -Dindex=strchr -Drindex=strrchr
OBJS	= include.o

include: $(OBJS)
	cc -o include $(OBJS)

lint:
	lint $(OBJS:.o=.c) | tee lint.out

clean:
	rm -f core *.o include a.out
