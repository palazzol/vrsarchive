SHELL	= /bin/sh
CFLAGS	= -i -O -K
USRLCL	= $(ROOT)/usr/local

crack:	crack.o
	cc $(CFLAGS) -o $@ crack.o

clean:
	rm -f crack crack.o

$(USRLCL):
	mkdir $@
	chmod 755 $@

install: $(USRLCL)
	mv crack $(USRLCL)
	chmod 711 $(USRLCL)/crack

crack.o: crack.c crack.h
