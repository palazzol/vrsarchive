ALL		= cbuf termlook
include	/etc/CONFIG

install: $(ALL) $(BINDIR)
	cp $(ALL) $(BINDIR)
	cd $(BINDIR); chmod 4711 $(ALL)

clean:
	rm -f $(ALL)
