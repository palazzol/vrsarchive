HDRS= mush.h config.h-dist strings.h bindings.h
SRCS1= main.c init.c misc.c execute.c
SRCS2= signals.c aliases.c msgs.c pick.c viewopts.c
SRCS3= sort.c expr.c folders.c dates.c help.c
SRCS4= loop.c bind.c
SRCS5= commands.c setopts.c hdrs.c
SRCS6= mail.c print.c
SRCS7= curses.c curs_io.c
OBJS= main.o init.o misc.o mail.o hdrs.o execute.o commands.o print.o \
      signals.o aliases.o setopts.o msgs.o pick.o sort.o expr.o \
      folders.o dates.o loop.o help.o viewopts.o bind.o curses.o curs_io.o
DOCS= README cmd_help mush.1

CFLAGS= -DSYSV -Mle -DCURSES
LDFLAGS= -X -Mle -lx -F 3000 -lcurses -ltermlib

mush: $(OBJS)
	@echo loading...
	@cc $(LDFLAGS) $(OBJS) -o mush

misc.o:	misc.c
	cc $(CFLAGS) -LARGE -c misc.c

bind.o:	bind.c
	cc $(CFLAGS) -LARGE -c bind.c

shar:
	shar makefile.sys.v makefile.bsd ${DOCS} ${HDRS} > hdr.shr
	shar ${SRCS1} > src1.shr
	shar ${SRCS2} > src2.shr
	shar ${SRCS3} > src3.shr
	shar ${SRCS4} > src4.shr
	shar ${SRCS5} > src5.shr
	shar ${SRCS6} > src6.shr
	shar ${SRCS7} > src7.shr

tar:
	tar fcv MUSH makefile.sys.v ${HDRS} ${DOCS} ${SRCS1} ${SRCS2} ${SRCS3} \
	${SRCS4} ${SRCS5} ${SRCS6} ${SRCS7}

clean:
	rm -f *.o core mush
