#
#	$Header: /home/Vince/cvs/local.d/news.d/Makefile,v 1.2 1987-02-15 15:54:57 vrs Exp $
#
SUBDIR	= src man
ALL	= ${SUBDIR}
include ../../CONFIG
ALIASES	= $(LIBMAIL)/aliases
NEWALIAS= $(LIBMAIL)/aliashash
#
# Programs that live in subdirectories, and have makefiles of their own.
#
${SUBDIR}: FRC
	@cd $@; make makefile; make ${MFLAGS}

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
