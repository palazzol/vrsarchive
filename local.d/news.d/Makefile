#	%W%	(Berkeley)	%G%
#
SHELL	= /bin/sh
ALIASES	= /usr/lib/mail/aliases
NEWALIAS= /usr/lib/mail/aliashash
#
# Programs that live in subdirectories, and have makefiles of their own.
#
SUBDIR=	src man

all:	${SUBDIR}

${SUBDIR}: FRC
	cd $@; make Makefile; make ${MFLAGS}

install:
	for i in ${SUBDIR}; do \
		(cd $$i; make ${MFLAGS} DESTDIR=${DESTDIR} install); \
	done
	if grep usenet $(ALIASES) >/dev/null 2>&1; then true; else \
		(umask 022; echo "usenet: root" >> $(ALIASES);$(NEWALIAS) ); \
	fi

clean:
	for i in ${SUBDIR}; do (echo $$i; cd $$i; make ${MFLAGS} clean); done

FRC:
