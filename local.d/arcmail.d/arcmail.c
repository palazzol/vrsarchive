/* arcmail - archive a mailbox
 *
 * usage: pmbox [filename]
 *
 * arcmail will take a berkeley mailbox and parse out the various messages.
 * It will then store the messages in that file into the directory ARCDIR
 * in the name ARCDIR/<account>/<date>, appending to an existing file.
 *
 * if [filename] is not given, the default name is defined by DEFMBOX.
 * 
 * if the From line has the same account name as USER then it will use
 * the account it finds in 'To:' since it is outgoing mail.
*/

#include <stdio.h>
#define ARCDIR "/usr/vrs/mail/old"
#define DEFMBOX "/usr/vrs/mail/box"
#define USER "vrs"

#define FALSE 0
#define TRUE 1
#define MAXLINE 512

#define ISFROM(x) ((!strncmp(x,"From ",5)) || (!strncmp(x,">From ",5)))
#define ISFROM_COLON(x) ((!strncmp(x,"From: ",6)) || (!strncmp(x,">From: ",5)))
#define ISTO(x) (!strncmp(x,"To: ",4))
#define ISUSER(x) (!strncmp(x,USER,strlen(USER)))
#define ISSUBJECT(x) (!strncmp(x,"Subject: ",9))

char line[MAXLINE];
char filename[MAXLINE];
char from[MAXLINE];
char from_colon[MAXLINE];
char to[MAXLINE];
char subject[MAXLINE];

main(argc,argv)
int argc;
char *argv[];
{
    FILE *fclose(), *fopen();

    if (argc <= 1) {
	if (freopen(DEFMBOX,"r",stdin)==NULL) {
	    fprintf(stderr,"%s: cannot open '%s'\n",argv[0],DEFMBOX);
	    exit(1);
	} else {
	    strcpy(filename,DEFMBOX);
	    process();
	    fclose(fopen(DEFMBOX,"w+")); /* truncate file */
	}
    } else {
	while (argc > 1) {
	    if (freopen(argv[1],"r",stdin)==NULL)
		fprintf(stderr,"%s: cannot open '%s'\n",argv[0],argv[1]);
	    else {
		strcpy(filename,argv[1]);
		process();
	    }
	    argc--; argv++;
	}
    }
    exit(0);
}

process()
{
    FILE *outfp, *make_file();
    int rc;

    from[0] = from_colon[0] = to[0] = subject[0] = '\0';

    if (gets(line) == NULL) {
	fprintf(stderr,"0 length file in %s\n",filename);
	return;
    }
    if (!ISFROM(line)) {
	fprintf(stderr,"%s is not in mbox format\n",filename);
	return;
    }
    strcpy(from,line); /* read_header assumes From is already read */

    do {
	from_colon[0] = to[0] = subject[0] = '\0';

	if (read_header() == EOF)
	    return;
	outfp = make_file();
	if (outfp) {
	    rc = write_message(outfp);
	    fclose(outfp);
	}
    } while (rc != EOF);
}

read_header()
{
    while (gets(line) != NULL) {
	if(!strlen(line)) /* blank line == end of header */
	    return(0); 
	else if(ISFROM_COLON(line))
	    strcpy(from_colon,line);
	else if(ISTO(line))
	    strcpy(to,line);
	else if(ISSUBJECT(line))
	    strcpy(subject,line);
	/* else skip unneeded header */
    }
    fprintf(stderr,"early EOF in read_header in %s\n",filename);
    return(EOF);
}

write_message(fp)
FILE *fp;
{
    int is_blank = FALSE;

    fprintf(fp,"%s\n",from);
    fprintf(fp,"%s\n",from_colon);
    fprintf(fp,"%s\n",to);
    fprintf(fp,"%s\n\n",subject);

    while(gets(line) != NULL) {
	if (!strlen(line)) {
	    is_blank = TRUE;
	    putc('\n',fp);
	} else if (is_blank && ISFROM(line)) {
	    strcpy(from,line);
	    return;
	} else {
	    is_blank = FALSE;
	    fprintf(fp,"%s\n",line);
	}
    }
    return(EOF);
}

FILE *make_file()
{
    char user[MAXLINE];
    char month[MAXLINE];
    char junk[MAXLINE];
    char junk2[MAXLINE];
    char *p;
    int day;
    int bangs = 0;
    char name[MAXLINE];
    FILE *fp;

	/*
	 *parse user and date from From 
	 */
    sscanf(from,"%s %s %s %s %d",junk,user,junk2,month,&day);
	/*
	 * if user == USER, parse user from To:
	 * because this is outgoing and we want the filename to 
	 * show where it is going, since it is coming from me.
	 */
    if (ISUSER(user))
	sscanf(to,"%s %s",junk,user);
	/*
	 * remove excess from the user field and then convert ugly 
	 * chars to dots
	 *
	 * algorithm: starting at end of string, walk backwards to
	 * one of:
	 *   beginning of string
	 *   two bangs
	 *   one @ and one bang.
	 * turning the chars '!@' to '.'
	 */

    p = &user[strlen(user)-1];
    while (p > &user[0]) 
    {
	if (index("!@", *p)) {
	    bangs++;
	    *p = '.';
	}

	if (bangs == 2) {
	    p++;
	    break;
	}
	p--;
    }
	/* 
	 * build the filename in the form ARCDIR/monthday/user
	 */ 
    sprintf(name,"%s/%s%d/%s",ARCDIR,month,day,p);
    mkparents(name);
	/* 
	 * open filename for append 
	 */
    if ((fp = fopen(name,"a+")) == NULL)
    {
	fprintf(stderr,"Can't open %s in append mode\n",name);
	return(NULL);
    }
    return(fp);
}

/*
 * If any parent directories of this dir don't exist, create them.
 */
mkparents(dirname)
char *dirname;
{
	char buf[200], sysbuf[200];
	register char *p;
	char *rindex();
	int rc;
	struct passwd *pw;

	strcpy(buf, dirname);
	p = rindex(buf, '/');
	if (p)
		*p = '\0';
	if (access(buf,0) == 0)
		return 0;
	mkparents(buf);
	sprintf(sysbuf, "mkdir %s", buf);
	rc = system(sysbuf);
	chmod(sysbuf, 0755);

	return rc;
}
