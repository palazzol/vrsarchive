# Do not define SIII or XENIX if you have v7 or BSD
CFLAGS	= # -DSIII -DXENIX
CFLAGS	= -DXENIX
DEST	= /usr/local
LDFLAGS	= -lx
OBJECTS	= vttest.o
SHELL	= /bin/sh

vttest: $(OBJECTS)
	$(CC) $(LDFLAGS) -o vttest $(OBJECTS)

install: vttest
	cp vttest $(DEST)
	chmod 711 $(DEST)/vttest

clean:
	rm -f $(OBJECTS) vttest core make.out
