XBM		= ../xbm/*.xbm
ALL		= `for i in $(XBM); do echo \`basename $$i .xbm\`.imp; done`
.SUFFIXES: .xbm .imp

all:
	make $(ALL)

clean:
	cd ../imp; rm -f $(ALL)
	rm -f $(ALL)

install: all
	cd ../imp; rm -f $(ALL)
	ln *.imp ../imp

#
#	File translation rules:
#	.xbm 	Bitmap format for X11
#	.imp 	Bitmap format for Imagen
#
.xbm.imp:
	@cd ../src; make all
	../src/xbm2imp <$< >tmp
	mv tmp $@
