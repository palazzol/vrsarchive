ALL		= maptoX maptoXrgb
CMAP	= Xrgb.gif Xcolors.gif
PAGES	= #maptoX.1 maptoXrgb.1
include /etc/CONFIG
.SUFFIXES: .uu .gif

all:	$(ALL) $(CMAP)

.sh:
	sed 's:^LIBDIR=.*$$:LIBDIR=$(LIBDIR):' $< >$@+
	chmod 755 $@+
	mv $@+ $@

.uu.gif:
	uudecode $<

clean:
	rm -f $(ALL) $(CMAP)

install: $(ALL) $(CMAP) $(MAN1) $(BINDIR) $(LIBDIR)
	cp $(ALL) $(BINDIR)
	cd $(BINDIR); chmod 755 $(ALL)
	cp $(CMAP) $(LIBDIR)
	cd $(LIBDIR); chmod 644 $(CMAP)
#	cp $(PAGES) $(MAN1)
#	cd $(MAN1); chmod 644 $(PAGES)
