/*
** default news poster
*/
#define DEF_POST "/usr/lib/news/inews -h"

/*
** default user .newsrc file
*/
#define DEF_NEWSRC ".newsrc"

/*
** If INLETTER is defined, the address line will be placed into the
** file editted by the user, and the mailer is assumed smart enough
** to understand about header lines in the file.  Otherwise the
** address is part of the mailer's command line.
**
** if MAILSMART is defined, The From: line will be used for mail replies,
** or overridden by a "Reply-to:" line if present - "Path:" will be used
** as a last resort.  If MAILSMART is not defined, "Path:" will simply be
** used.
**
** if MAILCHOOSE is defined, the user is prompted before edit with all
** of the address lines to choose from, or to input a new one.  MAILCHOOSE
** makes MAILSMART irrelevant, but the two are independent of INLETTER.
**
#define MAILCHOOSE
*/
#define MAILSMART
#define INLETTER

/*
** default mail sender.  If INLETTER, will be done as
** cat <file> | DEF_MAIL,  Otherwise, cat <file> | DEF_MAIL <address>
** user's MAILER variable will have to conform, too.
*/
#ifdef INLETTER
#define DEF_MAIL "/usr/lib/sendmail -t"
#else
#define DEF_MAIL "/bin/mail"
#endif

/*
** OLDRC defined for an apparently earlier news version which took unnamed
** command line options as synonyms for -n, and did not take ranges in
** the .newsrc file.  Probably useless, but kept in for historical reasons.
**
**#define OLDRC
*/

/*
** article spool directory
*/
#define SPOOLDIR "/usr/spool/news"

/*
** active file
*/
#define ACTFILE "/usr/lib/news/active"

/*
** maximum number of option lines in .newsrc
*/
#define OPTLINES 60

/*
** maximum number of filter options
*/
#define NUMFILTER 30

/*
** maximum number of file lines to search looking for header lines.
*/
#define HDR_LINES 36

/*
** When a newsgroup is scanned, we ignore articles less than <high spool> -
** MAXARTRANGE.  This is intended to prevent ridiculous numbers of article
** opening attempts the first time a user reads a new newsgroup which has a
** huge difference between the high and low spool numbers, perhaps due to
** some articles not getting expired.
*/
#define MAXARTRANGE 1600	/* about 2 weeks of soc.singles */

/*
** If we detect that the user has a higher number in .newsrc than the
** high article number, obviously the active file is out of synch with the
** .newsrc.  We set the user's number back to the low article number in
** this case, on the theory that it's better to repeat stuff than miss
** articles.  On such setbacks, we won't backdate the user by more than
** SYN_SETBACK articles, preventing floods of articles on large newsgroups
** if you don't define SYN_CHECK, the user's number won't be adjusted in
** this case, choosing to lose articles rather than show old ones.
*/
#define SYN_CHECK
#define SYN_SETBACK 60
