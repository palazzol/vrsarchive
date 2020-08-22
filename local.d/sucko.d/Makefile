OPTIONS	= -g
ALL 	= sucklst$(EXE) sucko active$(EXE)
include $(HOME)/CONFIG

all:	$(ALL)

active$(EXE): active.o nntp.o
	$(CC) $(CFLAGS) -o $@ active.o nntp.o

sucklst$(EXE): main.o nntp.o
	$(CC) $(CFLAGS) -o $@ main.o nntp.o

sucko:	sucko.sh
	cp $? $@

clean:
	rm -f $(ALL) *.o

install: $(ALL)
	cp $(ALL) $(BINDIR)
	cd $(BINDIR); chmod 755 $(ALL)
