OPTIONS	= -g
ALL 	= sucklst sucko active
include $(HOME)/CONFIG

all:	$(ALL)

active: active.o nntp.o
	$(CC) $(CFLAGS) -o $@ active.o nntp.o

sucklst: main.o nntp.o
	$(CC) $(CFLAGS) -o $@ main.o nntp.o

clean:
	rm -f $(ALL) *.o

install: $(ALL)
	cp $(ALL) $(BINDIR)
	cd $(BINDIR); chmod 755 $(ALL)
