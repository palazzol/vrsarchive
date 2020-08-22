/*
 * All the article header values worth retaining.
 *
 * All members of hdrs must point at malloced memory so that freeheaders
 * can free it without knowledge of what's malloced and what's static.
 * Furthermore, each member of hdrs must point at its own private copy
 * of its value string, for the above reason, and no other part of inews
 * may copy any member nor a modified copy of any member.
 * Perhaps C++ will allow this to be enforced by a strings class.
 */
struct headers {
	char *h_subj;		/* subject - only needed for controls */
	char *h_ngs;		/* newsgroups */
	char h_files[MAXLINE];	/* file names for history */
	char *h_distr;		/* distribution for transmit */
	char *h_ctlcmd;		/* control command */
	char *h_approved;	/* needed for acceptance in moderated groups */
	char *h_msgid;		/* needed for history & rejection */
	char *h_artid;		/* needed for history & rejection (obs.) */
	char *h_expiry;		/* needed for history */
	char *h_path;		/* needed for transmit - must munge */
	char h_tmpf[MAXFILE];	/* temp link name or first spool dir link */
	char h_unlink;		/* flag: true iff h_tmpf should be unlinked */
	char h_filed;		/* flag: true iff article has been filed */
	char h_xref;		/* flag: true iff Xref: header generated yet */
	char h_octlchked;	/* flag: true iff ngs checked for all.all.ctl */
	char h_oldctl;		/* flag: true iff ngs were all.all.ctl when checked */
	char *h_accum;		/* accumulated headers, if any */
	unsigned h_bytesleft;	/* in h_accum */
	long h_charswritten;	/* into spool directory, for batcher */
};
