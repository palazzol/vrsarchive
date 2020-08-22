# Makefile for Unix/Xenix rz and sz programs
# the makefile is not too well tested yet
EXE=.exe
ALL	= rz$(EXE) sz$(EXE)
include $(HOME)/CONFIG

rz$(EXE):
	$(CC) rz.c -o rz$(EXE)
	size rz$(EXE)

sz$(EXE):
	$(CC) -DSV -O -DNFGVMIN sz.c -o sz$(EXE)
	size sz$(EXE)

clean:
	rm -f $(ALL)

install: $(ALL) $(BINDIR) $(MAN1)
	cp $(ALL) $(BINDIR)
	cd $(BINDIR); chmod 755 $(ALL)
	cd $(BINDIR); rm -f rb$(EXE) rx$(EXE) sb$(EXE) sx$(EXE)
	cd $(BINDIR); ln rz$(EXE) rb$(EXE)
	cd $(BINDIR); ln rz$(EXE) rx$(EXE)
	cd $(BINDIR); ln sz$(EXE) sb$(EXE)
	cd $(BINDIR); ln sz$(EXE) sx$(EXE)
	cp rz.1 sz.1 $(MAN1)
	cd $(MAN1); chmod 644 rz.1 sz.1

nothing:
	@echo
	@echo "Please study the #ifdef's in rbsb.c, rz.c and sz.c,"
	@echo "then type 'make system' where system is one of:"
	@echo "	sysvr3	SYSTEM 5.3 Unix with mkdir(2)"
	@echo "	sysv	SYSTEM 3/5 Unix"
	@echo "	xenix	Xenix"
	@echo "	x386	386 Xenix"
	@echo "	bsd	Berkeley 4.x BSD, Ultrix, V7"
	@echo

usenet:
	shar -f /tmp/rzsz README Makefile zmodem.h zm.c sz.c rz.c rbsb.c \
	  minirb.c *.1 gz ptest.sh zupl.t

shar:
	 shar -f /tmp/rzsz -m 1000000 README Makefile zmodem.h zm.c \
	    sz.c rz.c rbsb.c minirb.c *.1 gz ptest.sh zupl.t

unixforum: shar
	compress -b12 /tmp/rzsz.sh

arc:
	rm -f /tmp/rzsz.arc
	arc a /tmp/rzsz README Makefile zmodem.h zm.c sz.c rz.c \
	    rbsb.c *.1 gz ptest.sh zupl.t minirb.c
	chmod og-w /tmp/rzsz.arc
	mv /tmp/rzsz.arc /t/yam

zoo:
	rm -f /tmp/rzsz.zoo
	zoo a /tmp/rzsz README Makefile zmodem.h zm.c sz.c rz.c \
	    rbsb.c *.1 gz ptest.sh zupl.t minirb.c
	chmod og-w /tmp/rzsz.zoo
	mv /tmp/rzsz.zoo /t/yam

.PRECIOUS:rz sz

xenix:
	$(CC) -M0 -Ox -K -i -DNFGVMIN -DREADCHECK sz.c -lx -o sz
	size sz
	-ln sz sb
	-ln sz sx
	$(CC) -M0 -Ox -K -i -DMD rz.c -o rz
	size rz
	-ln rz rb
	-ln rz rx

x386:
	$(CC) -Ox -DMD rz.c -o rz
	size rz
	-ln rz rb
	-ln rz rx
	$(CC) -Ox -DNFGVMIN -DREADCHECK sz.c -lx -o sz
	size sz
	-ln sz sb
	-ln sz sx

sysv:
	$(CC) -O -DMD rz.c -o rz
	size rz
	-ln rz rb
	-ln rz rx
	$(CC) -DSV -O -DNFGVMIN sz.c -o sz
	size sz
	-ln sz sb
	-ln sz sx

sysvr3:
	$(CC) -O -DMD=2 rz.c -o rz
	size rz
	-ln rz rb
	-ln rz rx
	$(CC) -DSV -O -DNFGVMIN sz.c -o sz
	size sz
	-ln sz sb
	-ln sz sx

bsd:
	$(CC) -DMD=2 -Dstrchr=index -DV7 -O rz.c -o rz
	size rz
	-ln rz rb
	-ln rz rx
	$(CC) -DV7 -O -DNFGVMIN sz.c -o sz
	size sz
	-ln sz sb
	-ln sz sx

sz: nothing
sb: nothing
rz: nothing
rb: nothing
