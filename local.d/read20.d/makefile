#
#	%W%	%G%
#
SHELL	= /bin/sh
OBJS	= read20.o
CFILES	= read20.c
HDRS	= dump.h
CFLAGS	= -O -Dindex=strchr -Drindex=strrchr
LFLAGS	=

all: read20

tags: $(HDRS) $(CFILES)
	ctags $(HDRS) $(CFILES)

install: read20
	cp read20 $(DESTDIR)/usr/games/read20
	chmod 711 $(DESTDIR)/usr/games/read20

read20.o: $(HDRS)

read20: $(OBJS)
	$(CC) $(LFLAGS) -o read20 $(OBJS)

clean:
	rm -f $(OBJS) read20 core make.out
