/* $Header: /home/Vince/cvs/news.d/rn.d/last.c,v 1.1 1990-02-26 22:32:15 vrs Exp $
 *
 * $Log: not supported by cvs2svn $
 * Version 1.1  87/12/29  10:18:49  vrs
 * Initial version
 * 
 * Revision 4.3  85/05/01  11:42:16  lwall
 * Baseline for release with 4.3bsd.
 * 
 */

#include "EXTERN.h"
#include "common.h"
#include "rn.h"
#include "util.h"
#include "intrp.h"
#include "INTERN.h"
#include "last.h"

char *lastname = Nullch;	/* path name of .rnlast file */

void
last_init(tcbuf)
char *tcbuf;
{
    lastname = savestr(filexp(LASTNAME));
    if ((tmpfp = fopen(lastname,"r")) != Nullfp) {
	fgets(tcbuf,1024,tmpfp);
	tcbuf[strlen(tcbuf)-1] = '\0';
	lastngname = savestr(tcbuf);
	fgets(tcbuf,1024,tmpfp);
	lasttime = atol(tcbuf);
	fgets(tcbuf,1024,tmpfp);
	lastactsiz = atol(tcbuf);
	fclose(tmpfp);
    }
    else {
	lastngname = nullstr;
	lasttime = 0;
	lastactsiz = 0;
    }
}

/* put out certain values for next run of rn */

void
writelast()
{
    if ((tmpfp = fopen(lastname,"w")) != Nullfp) {
	fprintf(tmpfp,"%s\n%ld\n%ld\n",ngname,(long)lasttime,(long)lastactsiz);
	fclose(tmpfp);
    }
    else
	printf(cantcreate,lastname) FLUSH;
}
