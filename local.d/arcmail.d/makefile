#
# arcmail %W%	%G%
#
#
SHELL	= /bin/sh
CFLAGS	= -Dindex=strchr -Drindex=strrchr
OBJS	= arcmail.o

arcmail: $(OBJS)
	cc -o arcmail $(OBJS)

lint:
	lint $(OBJS:.o=.c) | tee lint.out

clean:
	rm -f core *.o arcmail a.out
