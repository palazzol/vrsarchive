#
# Configuration options
#  This is the directory where the letter files will be installed.
#
OPTIONS	= -O
MODEL	= -i
ALL	= letter ret$(EXE)
include $(HOME)/CONFIG
OBJS	= ret.o
SRCS	= $(OBJS:.o=.c) $(SOBJS:.o=.c)

ret$(EXE):	$(OBJS)
	$(CC) -o $@ $(CFLAGS) $(OBJS) $(LIBS)

.sh:
	echo '#!/bin/sh' >$@+
	echo GAMLIB=$(GAMLIB) >>$@+
	echo PAGER=$(PAGER) >>$@+
	cat $? >> $@+
	mv $@+ $@

$(OBJS): words.h

install: letter ret $(GAMDIR) $(GAMLIB)
	cp ret$(EXE) $(GAMLIB)/letter$(EXE)
	chmod 711 $(GAMLIB)/letter$(EXE)
	cp letter $(GAMDIR)
	chmod 755 $(GAMDIR)/letter

clean:
	rm -f $(OBJS) ret$(EXE) letter core make.out
