#
#	$Header: /home/Vince/cvs/local.d/less.d/makefile,v 1.3 1990-03-30 17:03:01 vrs Exp $
#
ALL	= less
OPTIONS = "-Doff_t=$(off_t)" "-DVOID=$(VOID)" \
	  "-DREGCMP=$(REGCMP)" "-DRECOMP=$(RECOMP)" \
	  "-DSHELL_ESCAPE=$(SHELL_ESCAPE)" \
	  "-DEDITOR=$(EDITOR)" "-DEDIT_PGM=\"$(EDIT_PGM)\"" \
	  "-DONLY_RETURN=$(ONLY_RETURN)" \
	  "-DOLD_PAGER_NEW_LOCATION=\"$(OLD_PAGER_NEW_LOCATION)\"" -O
MODEL	= -i -F 2000
include /etc/CONFIG
#
# Makefile for "less"
#
# Invoked as:
#	make all
#   or	make install
# Plain "make" is equivalent to "make all".
#
# If you add or delete functions, remake funcs.h by doing:
#	make newfuncs
# This depends on the coding convention of function headers looking like:
#	" \t public <function-type> \n <function-name> ( ... ) "
#
# Also provided:
#	make lint	# Runs "lint" on all the sources.
#	make clean	# Removes "less" and the .o files.
#	make clobber	# Pretty much the same as make "clean".
#
#	make pager_patch		# makes PAGER environment variable
#	make install_pager_patch	# catcher and installs it (see below)
SHELL	= /bin/sh

##########################################################################
# System-specific parameters
##########################################################################

# VOID is 1 if your C compiler supports the "void" type,
# 0 if it does not.
VOID = 1

# off_t is the type which lseek() returns.
# It is also the type of lseek()'s second argument.
off_t = long

##########################################################################
# Optional and semi-optional features
##########################################################################

# REGCMP is 1 if your system has the regcmp() function.
# This is normally the case for System 5.
# RECOMP is 1 if your system has the re_comp() function.
# This is normally the case for BSD.
# If neither is 1, pattern matching is supported, but without metacharacters.
REGCMP = 1
RECOMP = 0

# SHELL_ESCAPE is 1 if you wish to allow shell escapes.
# (This is possible only if your system supplies the system() function.)
SHELL_ESCAPE = 1

# EDITOR is 1 if you wish to allow editor invocation (the "v" command).
# (This is possible only if your system supplies the system() function.)
# EDIT_PGM is the name of the (default) editor to be invoked.
EDITOR = 1
EDIT_PGM = /bin/vi

# parameters to "make install_pager_patch".  OLD_PAGER will be moved to
# OLD_PAGER_NEW_LOCATION and pager_patch (in this directory) will be installed
# as OLD_PAGER.  This patch will allow you to set the environment variable
# PAGER to specify your personal pager preference (is this a security hole?)
OLD_PAGER =
OLD_PAGER_NEW_LOCATION =

# ONLY_RETURN is 1 if you want RETURN to be the only input which
# will continue past an error message.
# Otherwise, any key will continue past an error message.
ONLY_RETURN = 0


##########################################################################
# Compilation environment.
##########################################################################

# INSTALL_LESS is a list of the public versions of less.
# INSTALL_MAN is a list of the public versions of the manual page.
INSTALL_LESS =	$(BINDIR)/less
INSTALL_MAN =	$(MAN1)/less.1

##########################################################################
# Files
##########################################################################

SRC1 =	main.c option.c prim.c 
SRC2 =	ch.c position.c input.c output.c screen.c \
	prompt.c line.c signal.c help.c ttyin.c command.c version.c
SRC =	$(SRC1) $(SRC2)
OBJ =	main.o option.o prim.o ch.o position.o input.o output.o screen.o \
	prompt.o line.o signal.o help.o ttyin.o command.o version.o


##########################################################################
# Rules
##########################################################################

less:	$(OBJ)
	$(CC) $(CFLAGS) -o $@ $(OBJ) $(LIBS)

install: $(ALL) $(BINDIR) $(MAN1)
	for f in $(INSTALL_MAN); do rm -f $$f;cp less.l $$f;chmod 644 $$f; done
	for f in $(INSTALL_LESS); do rm -f $$f;cp less $$f;chmod 711 $$f; done

pager_patch: pager_patch.c
	$(CC) $(CFLAGS) -s -o $@ pager_patch.c

install_pager_patch: pager_patch
	if [ -s $(OLD_PAGER) -a ! -s $(OLD_PAGER_NEW_LOCATION) ]; then \
		mv $(OLD_PAGER) $(OLD_PAGER_NEW_LOCATION); \
		cp pager_patch $(OLD_PAGER); \
	fi
	touch install_pager_patch

$(OBJ): less.h funcs.h

lint:
	lint -hp $(CFLAGS) $(SRC)

newfuncs:
	mv funcs.h funcs.h.OLD
	awk -f mkfuncs.awk $(SRC) >funcs.h

clean:
	rm -f $(OBJ) less pager_patch

clobber:
	rm -f *.o less pager_patch install_less install_man install_pager_patch

shar:
	shar -v INSTALLATION less.l makefile.* *.h *.awk $(SRC1) > less.shar.a
	shar -v $(SRC2) pager_patch.c > less.shar.b
