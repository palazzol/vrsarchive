###
# This program written by ADE Muffett (aem@aber.ac.uk), August 1991,
# as part of the 'Crack' password cracking package.
###

SD=	../Sources

source:	crack-pp crack-pwc
	touch source

crack-pp: $(SD)/crack-pp.c $(SD)/crack-lib.c $(SD)/conf.h
	( cd $(SD) ; make crack-pp )
	cp $(SD)/crack-pp .

crack-pwc: $(SD)/crack-pwc.c $(SD)/crack-lib.c $(SD)/conf.h $(SD)/crack-fcrypt.c
	( cd $(SD) ; make crack-pwc )
	cp $(SD)/crack-pwc .
