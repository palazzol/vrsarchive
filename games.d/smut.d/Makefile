SHELL	= /bin/sh
#
# Configuration options
#  This is the directory where the letter files will be installed.
#
BINDIR	= $(DESTDIR)/usr/games
LIBDIR	= $(BINDIR)/lib
CFLAGS	= -i -O
OBJS	= ret.o
SRCS	= $(OBJS:.o=.c) $(SOBJS:.o=.c)

ret:	$(OBJS)
	$(CC) -o $@ $(CFLAGS) $(OBJS)

$(OBJS): words.h

$(BINDIR) $(LIBDIR):
	mkdir $@
	chmod 755 $@

install: letter ret $(BINDIR) $(LIBDIR)
	cp ret $(LIBDIR)/letter
	chmod 711 $(LIBDIR)/letter
	cp letter $(BINDIR)
	chmod 755 $(BINDIR)/letter

clean:
	rm -f $(OBJS) ret letter core make.out
