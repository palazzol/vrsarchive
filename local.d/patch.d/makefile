#
#	$Header: /home/Vince/cvs/local.d/patch.d/makefile,v 1.9 2002-11-23 07:51:55 Vincent Exp $
#
# $Log: not supported by cvs2svn $
# Revision 1.8  1990/03/30 17:03:14  vrs
# Move CONFIG to /etc
#
# Version 1.7  87/02/15  15:58:02  vrs
# Use CONFIG
# 
# Version 1.6  86/12/23  14:17:21  vrs
# Check in patch version 2.0
# 
# Revision 2.0  86/09/17  15:36:15  lwall
# Baseline for netwide release.
# 
# Revision 1.2  86/09/08  14:07:42  lwall
# Split up patch.c.
# 
# Revision 1.1  86/08/01  20:18:35  lwall
# Initial revision
# 
MODEL	= -i
OPTIONS	= -O
public	= patch
private	= 
ALL	= $(public) $(private)
include $(HOME)/CONFIG
manext = 1
manpages = patch.man

c = patch.c pch.c inp.c version.c util.c

obj = patch.o pch.o inp.o util.o version.o

lintflags = -phbvxac

addedbyconf = Makefile.old bsd config.h config.sh eunice loc pdp11 usg v7

.c.o:
	$(CC) -c $(CFLAGS) $*.c

patch: $(obj)
	$(CC) $(CFLAGS) $(obj) $(libs) -o patch

# won't work with csh
install: $(ALL)
	cp $(public) $(BINDIR)
	cd $(BINDIR); chmod 755 $(public)
	for page in $(manpages); do \
		cp $$page $(MAN1)/`basename $$page .man`.$(manext); \
	done

clean:
	rm -f *.o *.orig core $(ALL)

realclean:
	rm -f patch *.o *.orig core $(addedbyconf)

# The following lint has practically everything turned on.  Unfortunately,
# you have to wade through a lot of mumbo jumbo that can't be suppressed.
# If the source file has a /*NOSTRICT*/ somewhere, ignore the lint message
# for that spot.

lint:
	lint $(lintflags) $(defs) $(c) > patch.fuzz

patch.o: config.h common.h patch.c inp.h pch.h util.h version.h
pch.o: config.h common.h pch.c pch.h util.h
inp.o: config.h common.h inp.c inp.h util.h
util.o: config.h common.h util.c util.h
version.o: config.h common.h version.c version.h patchlevel.h util.h

