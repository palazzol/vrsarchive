OBJS=main.o plot.o sub1.o sub2.o termio.o

a.out:	$(OBJS)
	$(CC) $(OBJS) -lm -ltermlib

.c.o:
	cc -O -c $*.c

$(OBJS): vtrek.h
