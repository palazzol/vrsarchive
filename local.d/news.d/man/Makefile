DEST	= ${DESTDIR}/usr/man/manl

all:;

makefile: Makefile

Makefile:;

$(DEST):
	mkdir $@
	chmod 755 $@

install: $(DEST)
	cp checknews.1 $(DEST)/checknews.n
	chmod 644 $(DEST)/checknews.n
	cp inews.1 $(DEST)/inews.n
	chmod 644 $(DEST)/inews.n
	cp postnews.1 $(DEST)/postnews.n
	chmod 644 $(DEST)/postnews.n
	cp readnews.1 $(DEST)/readnews.n
	chmod 644 $(DEST)/readnews.n
	cp newsrc.5 $(DEST)/newsrc.n
	chmod 644 $(DEST)/newsrc.n
	cp expire.8 $(DEST)/expire.n
	chmod 644 $(DEST)/expire.n
	cp recnews.8 $(DEST)/recnews.n
	chmod 644 $(DEST)/recnews.n
	cp sendnews.8 $(DEST)/sendnews.n
	chmod 644 $(DEST)/sendnews.n
	cp uurec.8 $(DEST)/uurec.n
	chmod 644 $(DEST)/uurec.n

clean:;
