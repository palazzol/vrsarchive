ALL	= cbw decrypt enigma zeecode
OPTIONS	= -O
MODEL	= -M2l
CBWLIB	= $(LIBDIR)/cbw
WRDLST	= /usr/dict/words
include /etc/CONFIG

# Object files for for the workbench.
CBOBJ	= pword.o lpair.o approx.o eclass.o cipher.o char-io.o tritab.o \
	  autotri.o pqueue.o webster.o user.o gblock.o dblock.o dbsaux.o \
	  banner.o cblocks.o stats.o parser.o knit.o pgate.o perm.o \
	  terminal.o keylib.o windowlib.o dline.o screen.o
ECTOBJ	= edriver.o eclass.o cipher.o char-io.o triglist.o webster.o user.o \
	  gblock.o dblock.o dbsaux.o banner.o cblocks.o trigram.o stats.o \
	  parser.o knit.o pgate.o perm.o keylib.o windowlib.o dline.o screen.o

# The main program.
cbw: start.o $(CBOBJ)
	$(CC) $(CFLAGS) $(CFLAGS) start.o $(CBOBJ) $(LIBS) -o $@

# Friendly interface to cbw
decrypt: decrypt.sh /etc/CONFIG Makefile
	sed '/^CBWLIB=/c\
CBWLIB=$(CBWLIB)' decrypt.sh > $@

# Program to encrypt files, this is identical to the
# Unix crypt function based on a two rotor enigma.
enigma: enigma.o
	$(CC) $(CFLAGS) $(CFLAGS) enigma.o -o $@

# Program to decrypt files after they have been broken by CBW.
zeecode: zeecode.o
	$(CC) $(CFLAGS) $(CFLAGS) zeecode.o -o $@

#
# The remaining files are test drivers.
#

bd: bdriver.o $(CBOBJ)
	$(CC) $(CFLAGS) bdriver.o $(CBOBJ) $(LIBS) -o $@

sd: sdriver.o $(CBOBJ)
	$(CC) $(CFLAGS) sdriver.o $(CBOBJ) $(LIBS) -o $@

approx: approx.o
	$(CC) $(CFLAGS) approx.o $(LIBS) -o $@

stats: stats.o char-io.o
	$(CC) $(CFLAGS) stats.o char-io.o $(LIBS) -o $@

tri: tdriver.o $(CBOBJ)
	$(CC) $(CFLAGS) tdriver.o $(CBOBJ) $(LIBS) -o $@

ect: $(ECTOBJ)
	$(CC) $(CFLAGS) $(ECTOBJ) $(LIBS) -o $@

ptt: char-io.o probtab.o
	$(CC) $(CFLAGS) char-io.o probtab.o $(LIBS) -o $@

dt: disptest.o keylib.o windowlib.o screen.o
	$(CC) $(CFLAGS) disptest.o keylib.o windowlib.o screen.o -o $@

triplace: triplace.o
	$(CC) $(CFLAGS) $(CFLAGS) triplace.o -o $@

$(CBWLIB):
	mkdir $@
	chmod 755 $@

clean:
	rm -f $(CBOBJ) start.o enigma.o zeecode.o bdriver.o sdriver.o
	rm -f $(ECTOBJ) approx.o stats.o char-io.o tdriver.o char-io.o
	rm -f $(ALL) probtab.o disptest.o triplace.o

install: $(ALL) $(BINDIR) $(LIBDIR) $(CBWLIB)
	cp enigma $(BINDIR)/crypt
	cp decrypt zeecode $(BINDIR)
	cd $(BINDIR); chmod 711 crypt zeecode
	cd $(BINDIR); chmod 755 decrypt
	cp $(WRDLST) $(CBWLIB)/word-list
	cp cbw mss.stats mss-bigram.stats trigrams.stats $(CBWLIB)
	cd $(CBWLIB); chmod 711 cbw
	cd $(CBWLIB); chmod 644 word-list mss.stats
	cd $(CBWLIB); chmod 644 mss-bigram.stats trigrams.stats 
