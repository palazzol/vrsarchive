MACROS	= tmac.emo tmac.key tmac.onth tmac.slide tmac.status
PAGES	= memo.7
include $(HOME)/CONFIG

clean:

install: $(ALL) $(LIBTMAC) $(MAN7)
	cp $(MACROS) $(LIBTMAC)
	cd $(LIBTMAC); chmod 644 $(MACROS)
	cp $(PAGES) $(MAN7)
	cd $(MAN7); chmod 644 $(PAGES)
