#
#	@(#)Makefile	1.1
#
SHELL	= /bin/sh
OBJS	= openfile.o ra1.o ra2.o rad1.o

ra:	$(OBJS)
	cc -o $@ $(OBJS)

$(OBJS): ra.h

clean:
	rm -f $(OBJS) core ra make.out
