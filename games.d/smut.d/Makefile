#
# Configuration options
#  This is the directory where the letter files will be installed.
#
OPTIONS	= -O
MODEL	= -i
ALL	= letter ret
include /etc/CONFIG
OBJS	= ret.o
SRCS	= $(OBJS:.o=.c) $(SOBJS:.o=.c)

ret:	$(OBJS)
	$(CC) -o $@ $(CFLAGS) $(OBJS) $(LIBS)

.sh:
	sed '/^GAMLIB/,/^PAGER/c\
GAMLIB=$(GAMLIB)\
PAGER=$(PAGER)' $? > $@

$(OBJS): words.h

install: letter ret $(GAMDIR) $(GAMLIB)
	cp ret $(GAMLIB)/letter
	chmod 711 $(GAMLIB)/letter
	cp letter $(GAMDIR)
	chmod 755 $(GAMDIR)/letter

clean:
	rm -f $(OBJS) ret letter core make.out
