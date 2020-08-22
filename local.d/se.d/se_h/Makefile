# 
# $Header: /home/Vince/cvs/local.d/se.d/se_h/Makefile,v 1.6 2002-11-25 02:21:20 vrs Exp $
# 
# $Log: not supported by cvs2svn $
# Revision 1.5  2002/11/23 19:01:57  Vincent
# Move CONFIG to $(HOME).
#
# Revision 1.4  1990/03/30 17:03:31  vrs
# Move CONFIG to /etc
#
# Version 1.3  87/02/13  10:15:55  vrs
# Use CONFIG
# 
# Version 1.2  87/02/07  20:34:40  vrs
# Fix $Header: /home/Vince/cvs/local.d/se.d/se_h/Makefile,v 1.6 2002-11-25 02:21:20 vrs Exp $
# 
# Revision 1.2  86/10/07  15:15:04  arnold
# Updated to reflect current set of help files.
# 
# Revision 1.1  86/05/06  14:04:17  osadr
# Initial revision
# 
# makefile for se help scripts
#
ALL	= detab$(EXE)
include $(HOME)/CONFIG
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
