#
#	@(#)Makefile	1.5 Date 87/05/09
#
CFLAGS	= -O
PFLAGS	= -O
FILES	= makefile ptc.p ptc.c ptc doc.ms doc.out ptc.man man.out
PAGELEN	= -rP70
DEV	= /dev/rst8
BLK	= 126

.DEFAULT:
	$(GET) -p -s SCCS/s.$@ > $@

# default version
S	= p
ptc:		ptc.$S
		$Sc -O -o ptc ptc.$S

# translated version
cptc:		cptc.c
		cc -O -o cptc cptc.c

cptc.c:		ptc.p ptc
		ptc < ptc.p > cptc.c

tests:	test0


#
#	Perform a simple test translating the source with the compiled
#	translator and the compairing its own source against its
#	translation of its own source. They should be exactly equal.
#
test0:	ptc cptc.c cptc FRC
	cptc < ptc.p > tmp.c
	cmp cptc.c tmp.c && rm tmp.c


FRC:

#
#	Documentation.
#
doc.out:	doc.ms
		tbl doc.ms | nroff -T$(TERM) $(PAGELEN) -ms - > doc.out

man.out:	ptc.man
		nroff -T$(TERM) $(PAGELEN) -man ptc.man > man.out


#
#	Maintenance
#
cleanup:
	rm -f cptc cptc.c tmp.c out

clean::	cleanup


clobber:: cleanup


tape:	$(FILES)
	tar cvfb $(DEV) $(BLK) $(FILES)
