# $Header: /home/Vince/cvs/local.d/patch.d/makefile,v 1.3 1986-10-05 12:00:50 root Exp $

# $Log: not supported by cvs2svn $
# Revision 1.3  85/03/26  15:03:26  lwall
# Frozen.
# 

DESTDIR = /usr/local

patch: patch.c
	cc patch.c -O -o patch
install:
	install -c patch $(DESTDIR)
clean:
	rm patch
shar:
	shar patch.man patch.c Makefile >patch.kit
lint:
	lint -phbvxac patch.c
