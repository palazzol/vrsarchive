#
# tpr %W%	%G%
#
#
SHELL	= /bin/sh
OBJS	= tpr0.o tpr1.o itoeng.o

tpr: $(OBJS)
	cc -o tpr $(OBJS)

lint:
	lint $(OBJS:.o=.c) | tee lint.out

clean:
	rm -f core *.o tpr a.out
