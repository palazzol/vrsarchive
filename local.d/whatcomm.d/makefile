#
# whatcomm %W%	%G%
#
#
SHELL	= /bin/sh
CFLAGS	= -i
OBJS	= whatcomm.o

whatcomm: $(OBJS)
	cc $(CFLAGS) -o whatcomm $(OBJS)

lint:
	lint $(OBJS:.o=.c) | tee lint.out

clean:
	rm -f core *.o whatcomm a.out
