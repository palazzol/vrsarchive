SHELL	= /bin/sh
CFLAGS	= -i # -Dvoid=int
BINDIR	= $(DESTDIR)/usr/local

install: patch
	cp patch $(BINDIR)
	cd $(BINDIR); chmod 711 patch

clean:
	rm -f core patch make.out

shar:
	shar patch.nr patch.c Makefile >patch.kit

lint:
	lint -phbvxac patch.c
