XBM		= ann01.xbm ann03.xbm ann04.xbm ann05.xbm ann19.xbm anncol.xbm \
		  asiagirl.xbm bbporn01.xbm bbporn02.xbm bbporn03.xbm bbporn04.xbm \
		  bbporn05.xbm bionca3.imp bionca3.xbm comb.xbm deedee.xbm \
		  doctor1.xbm doctor13.xbm doctor2.xbm doctor3.xbm doctor4.xbm \
		  doctor41.xbm doctor5.xbm doctor6.xbm foster1.xbm foster2.xbm \
		  foster4.xbm fractal.imp fractal.xbm master.xbm meanwho.xbm \
		  milk_it2.xbm multnma.imp multnma.xbm naval1.xbm naval3.xbm \
		  naval4.xbm ned.imp ned.xbm samfox.imp samfox.xbm scenic1.imp \
		  scenic1.xbm shawn.imp shawn.xbm simpsons.xbm tardis4.xbm \
		  trevor.imp trevor.xbm vrs.imp vrs.xbm
ALL		= $(XBM:.xbm=.imp)
.SUFFIXES: .xbm .imp

all:	$(ALL)

clean:
	cd ../imp; rm -f $(ALL)
	rm -f $(ALL)

install: $(ALL)
	cd ../imp; rm -f $(ALL)
	ln *.imp ../imp

#
#	File translation rules:
#	.xbm 	Bitmap format for X11
#	.imp 	Bitmap format for Imagen
#
.xbm.imp:
	@cd ../src; make all
	../src/xbm2imp $< >tmp
	mv tmp $@
