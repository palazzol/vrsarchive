ALL		= cbuf termlook
include	$(HOME)/CONFIG

install: $(ALL) $(BINDIR)
	cp $(ALL) $(BINDIR)
	cd $(BINDIR); chmod 4711 $(ALL)

clean:
	rm -f $(ALL)
