#
#	$Header: /home/Vince/cvs/local.d/recipes.d/Makefile,v 1.4 1987-02-15 20:39:28 vrs Exp $
#
ALL	= configure
include ../../CONFIG
# mod.recipes software Makefile for USG systems.
# To install, edit the following variables, then type "make install"
#
# Where are the object files for the recipe software kept?
OBJDIR	= $(BINDIR)
SHELL	= /bin/sh
# Where are the man pages to be put?
MAN1DIR=$(MAN1)
MAN5DIR=$(MAN5)

# Where is a temporary directory (e.g. /tmp or /usr/tmp) that will have
# enough space to hold the entire cookbook. It could require several
# megabytes someday.
TEMPDIR=/usr/tmp

# Where is the news spooling directory? Note: if your system is running a
# version of news earlier than B2.10, or some software besides news, you will
# have to edit the file "rckeepnew.X" and tell it the name of the directory
# that holds mod.recipes articles.
NEWSDIR=$(SPOOLN)

# What is the default "keep" directory. If you want a per-person keep
# directory, put $$HOME/something; if you want a systemwide keep directory
# (to save disk space), then put an absolute pathname here.
DEFDIR=$(LIBDIR)/Recipes

# What PATH do you want used inside the shell scripts? At the very least,
# it must contain $OBJDIR
DEFPATH=:$(OBJDIR):/usr/bin:/bin:.

# What version of TRoff works best at your site, and with what options does it
# do its best job on the Unix manual pages macros?
DEFTROFF=$(TROFF) -man

# What is your favorite pager and what are its options?
DEFPAGER=$(PAGER)

# Do you want recipes to come out in metric units (as opposed to English
# units)? If you think that tablespoons and cups are the right thing to have
# in recipes, then leave this set to "0". If you think that milliliters and
# grams are the right thing to have in recipes, then change to "1"
METRIC=0

############################################################################

OBJECTS=rcbook.t rcbook.n rcextract rcindex rckeep rcnroff rcshow rctypeset \
	rcintro rckeepnew rcnew.t rcnew.n

configure: Makefile configure.sh
	sh configure.sh USG '$(OBJDIR)' '$(NEWSDIR)' '$(DEFDIR)' '$(DEFPATH)' \
		'$(DEFTROFF)' '$(DEFPAGER)' '$(METRIC)' '$(TEMPDIR)'
	touch configure

$(DEFDIR):
	make $(LIBDIR)
	mkdir $@
	chmod 755 $@

clean:
	rm -f $(OBJECTS) *.1 *.5 *.n.X rcnroff.X *.tmp configure

install: configure $(OBJDIR) $(MAN1DIR) $(MAN5DIR) $(DEFDIR)
	cp $(OBJECTS) $(OBJDIR)
	cd $(OBJDIR); chmod 755 $(OBJECTS)
	cp cookbook.1 $(MAN1DIR)
	cd $(MAN1DIR); chmod 644 cookbook.1
	cp recipes.5 $(MAN5DIR)
	cd $(MAN5DIR); chmod 644 recipes.5
	cd Recipes; cp * $(DEFDIR)
	cd $(DEFDIR); chmod 644 *
