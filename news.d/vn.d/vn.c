/*
vn news reader for visual page oriented display of news
aimed at scanning large numbers of articles.

Original program by Bob McQueer in several versions 1983-1986.  Released
into the public domain in 1986.  While no copyright notice appears, the
original author asks that a history of changes crediting the proper people
be maintained.

Bob McQueer
{amdahl, sun, mtxinu, hoptoad, cpsc6a}!rtech!bobm

History:

	(bobm@rtech) 5/86 - first "public" version

	(bobm@rtech) 12/86 - version incorporates:

		bug fixes:
			str_store NULL string bug
			not picking up first article in newsgroup
			RESTART terminal reset for exit to editor.
			skip whitespace in "empty" digest lines while unpacking
			DISTRIBUTION line in followups.
			:100%: prompt on last line in reader.
			interpretation of multiple negations -w -t options.

			Many thanks to several people who noted the first
			two bugs as well as fixes.

			Thank you to Karl Williamson for helpful information
			tracking down the "concept terminal" bug (RESTART).

		SYSV ifdef's adapted from those done by Larry Tepper
		at ATT Denver - sent in by Karl Williamson, drutx!khw.
		Many people submitted SYSV ifdef's - thank you all.

		print capability from reader from Karl Williamson,
		drutx!khw

		Changes to use alternate header lines for mail, from changes
		by Andy Marrinson, andy@icom.UUCP (ihnp4!icom!andy).  Ifdef'ed
		to allow local configuration (bobm@rtech).

		"author_copy" file for followups, prevention of multiple
		"re: "'s, insert blank line and original author line
		before excerpted text from Andy Marrinson, andy@icom.UUCP

		Search string capability in reader, from Lawrie Brown,
		seismo!munnari!cdsadfa.oz!lpb (Australia).  Somewhat
		modified by interaction with the :100%: bug.

		Arrow key support, adapted from changes by Lawrie Brown.
		Modified to simply not allow control keys for arrows (allowing
		SOME controls is too prone to problems, esp. with .vnkey), and
		to allow the PAGEARROW ifdef (bobm@rtech)

		prevention of followups to "mod" and "announce", from
		Lawrie Brown.

		OLDRC ifdef adapted from changes by Lawrie Brown.  ADDRMUNGE
		added to allow OZ domain addressing changes from Australia
		to be grafted back in, and provide a hook for anybody else
		wanting to do something similar.

		Bob McQueer, bobm@rtech:

			a menu selection from the % command to jump to
			a new newsgroup

			linked list on hash table - no longer a compiled
			in limit for number of newsgroups

			.vnkey keystroke mapping file.

			options to get the % command list on entry, and to
			change how unsubscribed groups are handled for updates.

			allow configurable use of vs / ve pair for terminal
			handling.

	8/87 (bobm@rtech)

		Server interface changes.  Creation of vns_xxx routines
		formally defining how to attach vn to an abstracted
		news server, rearranged existing code to use that
		abstraction.  Sourcefile list altered a good bit by
		the rearrangement.  Some of this was splitting some
		routines out from vn.c into smaller modules.  Reader
		code was altered a good bit to work through the
		ARTHEADER abstraction, rather than searching for
		header lines.  Temp file writing code was the other
		area heavily affected.  The std.c server interface
		essentially incorporates the old newsrc.c code, plus
		the old outgroup() and gethead() routines.

		vns_changes also included:

			Modification of mailer interface for MAILCHOOSE,
			getting rid of ADDRMUNGE (superseded by vns
			interface definition)

			Moved readstr() into the session loop code,
			allowing a lot of static declarations to be
			placed therein.  Also made it possible to
			use strtok() underneath readstr().

			Made "save" write directly into file, rather
			than forking a cat (gee, that sounds perverse).

			Also fixes it so that "|" save convention works
			from reader.

			Digest unpacking has to know about header lines,
			unfortunately, so it has local definitions for
			some header lines.

		Incorporates:

			The much-discussed "continued header line" bug
			fix, of course.  Includes the multiple header
			line nfgets() by Andrew Worsley, andrew@munnari.oz,
			with a couple cosmetic changes.

			Top / Middle / Bottom keys from Karl Williamson
			print capability from reader from Karl Williamson,
			drutx!khw

			Edit old save file changes from George Pavel,
			gp@lll-lcc.arpa.  Used it to allow edit of
			the some other old strings, too.

			Fix for the bad number input bug on the choose
			new group from list feature, as reported by
			Dave Tallman, tallman@hc.arpa.  His fix with
			a few minor changes.

			A couple save file tweaks - allow a "%s" in VNSAVE
			to specify individual directories by newsgroup.
			Allow a "w:" prefix on filename to write instead
			of appending, allow %d for article number in name.

			The VNEDITOR variable.

			Statistics collection ability.

			Update of .newsrc "read" number to reflect removed
			articles, preventing rescanning of group on next
			session.

			take out SIGHUP catching to avoid problems with
			message being output.

			key to print version being used.
Known bugs:

	non-erasure of stuff on prompt line when the new
	string includes an escape sequence (like PS1 maybe)
	because it doesn't realize that the escape sequence
	won't overprint the existing stuff

	control-w and update on exit may not update pages which have been
	scanned in funny orders by jumping into the middle of groups

	inaccurate numbers on '%' command results - reflect ranges, not
	actual numbers of articles.

	no arrow keys recognized which don't begin with <escape>

	doesn't know about the version 2.11 'm' in active list, or
	use the 'y' / 'n' either.

	crash due to embedding $\(\) type substring specifiers in regular
	expressions.  Obscure and hard to fix in a proper and portable way.
*/
#include <stdio.h>
#include "node.h"
#include "tty.h"
#include "brk.h"

extern int Lrec;

extern NODE **Newsorder;
extern int Ncount;

extern int Listfirst, Nounsub;

main(argc,argv)
int argc;
char **argv;
{
	/*
		initialize environment variables,
		scan .newsrc file, using any command line options present.
	 */
	term_set (START);
	envir_set ();
	sig_set (BRK_IN);

	stat_start();

	hashinit();
	temp_open();

	vns_news (argc,argv,&Listfirst,&Nounsub);

	fw_done ();

	make_newsorder();

	tty_set (BACKSTOP);

	if (Lrec >= 0)
		session ();
	else
	{
		new_groups ();
		fprintf (stderr,"\nNo News\n");
	}

	tty_set (COOKED);

	/* exiting, don't worry about FLG_ECHG resetting */
	vns_write(Newsorder,Ncount);

	term_set (STOP);
	vns_exit(0);

	stat_end( Lrec >= 0 ? 1 : 0 );
}
