# 
# $Header: /home/Vince/cvs/local.d/se.d/se_h/Makefile,v 1.3 1987-02-13 10:15:55 vrs Exp $
# 
# $Log: not supported by cvs2svn $
# Version 1.2  87/02/07  20:34:40  vrs
# Fix $Header: /home/Vince/cvs/local.d/se.d/se_h/Makefile,v 1.3 1987-02-13 10:15:55 vrs Exp $
# 
# Revision 1.2  86/10/07  15:15:04  arnold
# Updated to reflect current set of help files.
# 
# Revision 1.1  86/05/06  14:04:17  osadr
# Initial revision
# 
# makefile for se help scripts
#
ALL	= detab
include ../../../CONFIG
DOCS	= README
FILES = a abt ah arg2 args b bang bug c cc ccmisc chardel charins colon \
	com comdir comsyn d dir e eh elp equal f g h i j k l list ln m \
	motion n o o- oa oc od of og oh oi ok ol olm om os ot ou ov ow \
	ox oy oz p pat patex q quote r rpt s scan serc shell specl t \
	termchar tilde u v w x y z

# destination for final	version	of help	scripts.
INSTALL	= $(LIBDIR)/se_h

install: existsall $(ALL)
	./Install $(INSTALL) $(FILES)

existsall: $(FILES) $(DOCS) Install
	@: all the files exist, otherwise make would die.

clean:
	rm -f $(ALL)

print:
	prt $(DOCS) Makefile Install $(FILES) | lpr -b 'se help'

print2:	$(DOCS)	Makefile Install $(FILES)
	prt $? | lpr -b	'new se	help'
