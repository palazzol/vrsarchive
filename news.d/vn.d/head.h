/*
** vn news reader.
**
** head.h - header line structure
**
** see copyright disclaimer / history in vn.c source file
*/

/*
** How this thing works:
**
** this structure is filled in by vns_aopen when opening an article.
** lines & hlines items will be used in providing percentage read prompts
**
** show_num & show are the article information lines presented for the user
** when the "show headers" flag is turned off.
**
** from and artid are used for mail salutations, etc.
**
** The items used for mail replies, FOLLOWING the call to the mail massager
** if there is one, are mailcmd, mail_num, and mail.  These are the items
** the massager should fill in.  If no massager exists, vns_aopen will
** fill these in directly.  If mail_err is non-null, the user won't be
** able to mail a reply to the article, and the item should be an error
** message explaining why.  If there is a mailer function, the mailcmd
** item is not used.
**
** The priv and priv_num items are for sole use of the server layer in
** the mail massager, mailer and poster functions.
**
** The postcmd, post_num, and post arguments are used in treatment of
** followups.  If post_err is non-null, followup won't be allowed, for
** the reason described therein.  If there is a poster function, the
** postcmd item isn't used.
**
** The header lines for inclusion in mail / followup files will open
** the file, and will be followed by one blank line.  The lines are literal -
** all appropriate headers should be prepended, etc.
**
** postcmd / mailcmd are used as format strings which are assumed to have a
** single %s in them some place for the placement of the users editted file.
** The result will be spawned as a command.
*/

typedef struct
{
	int lines;		/* number of lines in article */
	int hlines;		/* number of header lines in article */
	char *from;		/* authors name */
	char *artid;		/* article id */
	int show_num;		/* number of extra lines for reader display */
	char **show;		/* extra header lines */
	int priv_num;		/* number of private arguments */
	char **priv;		/* private server arguments */
	char *mail_err;		/* mail reply error message */
	char *mailcmd;		/* command line for mailer */
	int mail_num;		/* number of header lines in mail reply file */
	char **mail;		/* mail reply header lines */
	char *post_err;		/* follow-up posting error message */
	char *postcmd;		/* command line for followup poster */
	int post_num;		/* number of header lines for followup file */
	char **post;		/* followup header lines */
} ARTHEADER;
