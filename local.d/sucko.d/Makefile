OPTIONS	= -g
ALL 	= sucklst sucko
include $(HOME)/CONFIG

sucklst: main.o nntp.o
	$(CC) $(CFLAGS) -o $@ main.o nntp.o

clean:
	rm -f $(ALL) *.o

install: $(ALL)
	cp $(ALL) $(BINDIR)
	cd $(BINDIR); chmod 755 $(ALL)
