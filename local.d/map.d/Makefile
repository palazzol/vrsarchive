BIN	= $(DESTDIR)/usr/local
LIB	= $(BIN)/lib
LIBDIR	= $(LIB)/uucp
SHELL=/bin/sh

map:	region world makmap
	./makmap 

makmap:	makmap.c
	cc -O -o makmap makmap.c

$(BIN) $(LIB) $(LIBDIR):
	mkdir $@
	chmod 755 $@

install: $(BIN) $(LIB) $(LIBDIR) map
	cp map world $(LIB)
	chmod 644 $(LIB)/map $(LIB)/world

clean:
	rm -f makmap.o
