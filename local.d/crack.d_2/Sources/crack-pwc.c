#include "crack.h"

#ifdef hpux	/* You could add your particular unix variant here */
char rcmd[] = "remsh";
#else
char rcmd[] = "rsh";
#endif

char version[] = "3.3c";	/* version of prog */
char runtime[] = "Runtime";
char feedback_string[] = "!fb!";

/*
 * This program is copyright (c) Alec Muffett 1991 except for certain
 * portions of code ("crack-fcrypt.c") copyright (c) Robert Baldwin, Icarus
 * Sparry and Alec Muffett.  The author(s) disclaims all responsibility or
 * liability with respect to it's usage or its effect upon hardware or
 * computer systems.  This software is in freely redistributable PROVIDED
 * that this notice remains intact.
 */

/*
 * crack-pwc.c - an optimised password cracker. (c) ADE Muffett, July 1991.
 * Totally rewritten from an 'standard' password cracker that had been
 * floating about on ther network. Apart from the entire re-write, to save my
 * sanity, this thing orders passwords to minimise calls to crypt(). Use
 * fcrypt() where you have it.
 */

/* Structure definitions */

struct USER_ENTRY
{
    int done;			/* bool flag */
    char *filename;		/* where we got it from */
    char *passwd_txt;		/* plaintext of password */
    struct passwd passwd;	/* ...guess... */
    struct USER_ENTRY *across;	/* line of users with same salt */
    struct USER_ENTRY *next;	/* next users with different salt */
};

struct DICT_ENTRY
{
    char *word;			/* simple linked list */
    struct DICT_ENTRY *next;
};
/* Variable declarations */

struct USER_ENTRY *user_root;	/* root of the USER linked list */

int recover_bool;		/* we are trying to recover from a crash */
int verify_recover;		/* we will doublecheck usernames on recovery */
int rpid;			/* pid we are recovering */
char rdict[STRINGSIZE];		/* dict we are recovering */
char rusername[STRINGSIZE];	/* username we are recovering */
char rhostname[STRINGSIZE];	/* hostname we are recovering on */
int rusercount;			/* count of user we are recovering at;
				 * strictly, this is the line of the user we
				 * are recovering */

int verbose_bool;		/* is verbose switched on ? */

int pid;			/* pid of parent process */
char opfile[STRINGSIZE];	/* name of output file */
char diefile[STRINGSIZE];	/* name of diefile */
char pointfile[STRINGSIZE];	/* name of pointfile */
char feedbackfile[STRINGSIZE];	/* guess */
char hostname[STRINGSIZE];

/*
 * DIDDY LITTLE USEFUL FUNCTIONS
 */

void
Log (fmt, a, b, c, d, e, f, g, h, i, j)
    char *fmt;
{
    long t;

    time (&t);
    printf ("pwc: %-15.15s ", ctime (&t) + 4);
    printf (fmt, a, b, c, d, e, f, g, h, i, j);
    fflush (stdout);
}

void
CatchTERM ()
{
    /* bury magnets */
    Log ("Caught a SIGTERM! Commiting suicide...\n");
    /* swallow the rapture */
    Log ("<argh!>\n");
    /* let's gather feathers */
    sync ();
    /* - don't fall on me */
    exit (0);
}

void
SetPoint (dict, usernumber, username)	/* Sets the pointfile up */
    char *dict;
    int usernumber;
    char *username;
{
    FILE *fp;
    long t;

    if (!(fp = fopen (pointfile, "w")))
    {
	perror (pointfile);
	return;
    }
    time (&t);
#ifdef CRACK_NETWORK
    fprintf (fp, "%s\n", hostname);
#endif
    fprintf (fp, "%d\n", pid);
    fprintf (fp, "%s\n", dict);
    fprintf (fp, "%d\n", usernumber);
    fprintf (fp, "%s\n", username);
    fprintf (fp, "last pointed: %s", ctime (&t));
    fclose (fp);
}

int				/* returns 0 if OK, -1 if not recovering */
GetPoint (pf)
    char *pf;
{
    FILE *fp;
    char buffer[STRINGSIZE];

    if (!(fp = fopen (pf, "r")))
    {
	perror (pf);
	return (-1);
    }
#ifdef CRACK_NETWORK
    if (!fgets (buffer, STRINGSIZE, fp))	/* Get the hostname */
    {
	fprintf (stderr, "get hostname flopped\n");
	return (-1);
    }
    Trim (buffer);

    if (strcmp (buffer, hostname))	/* Not running on original host */
    {
	fprintf (stderr, "cmp hostname flopped\n");
	return (-1);
    }
    strcpy (rhostname, buffer);
#endif

    if (!fgets (buffer, STRINGSIZE, fp))	/* Get the pid */
    {
	fprintf (stderr, "get pid flopped\n");
	return (-1);
    }
    Trim (buffer);
    rpid = atoi (buffer);

    if (!fgets (buffer, STRINGSIZE, fp))	/* Get the dictname */
    {
	fprintf (stderr, "get dict flopped\n");
	return (-1);
    }
    Trim (buffer);
    strcpy (rdict, buffer);

    if (!fgets (buffer, STRINGSIZE, fp))
    {
	fprintf (stderr, "get usercount flopped\n");
	return (-1);
    }
    Trim (buffer);
    rusercount = atoi (buffer);	/* Get the usernumber to recover */

    if (!fgets (buffer, STRINGSIZE, fp))	/* Get the username */
    {
	fprintf (stderr, "get username flopped\n");
	return (-1);
    }
    Trim (buffer);
    strcpy (rusername, buffer);

    fclose (fp);
    return (0);
}
/*
 * STRUCTURE SPECIFIC FUNCTIONS
 */

char *
PWSkip (p)			/* jump the ':' in a pwent */
    register char *p;
{
    while (*p && *p != ':')
    {
	p++;
    }
    if (*p)
    {
	*p++ = '\0';
    }
    return (p);
}

struct USER_ENTRY *
Parse (buffer)			/* break up input into a structure */
    register char *buffer;
{
    register char *p;
    register struct USER_ENTRY *retval;

    retval = (struct USER_ENTRY *) malloc (sizeof (struct USER_ENTRY));
    retval -> next = retval -> across = NULL;
    retval -> passwd_txt = NULL;
    retval -> done = 0;
    Trim (buffer);

    p = Clone (buffer);
    retval -> filename = p;
    p = PWSkip (p);
    retval -> passwd.pw_name = p;
    p = PWSkip (p);
    retval -> passwd.pw_passwd = p;
    p = PWSkip (p);
    retval -> passwd.pw_uid = atoi (p);
    p = PWSkip (p);
    retval -> passwd.pw_gid = atoi (p);
    p = PWSkip (p);
    retval -> passwd.pw_gecos = p;
    p = PWSkip (p);
    retval -> passwd.pw_dir = p;
    p = PWSkip (p);
    retval -> passwd.pw_shell = p;
    return (retval);
}
/*
 * START OF MODULES
 */

void
LoadData ()			/* load sorted entries into memory */
{
    char *ptr;
    char salt[2];
    char buffer[STRINGSIZE];
    int numlines;
    int numentries;
    register struct USER_ENTRY *new_element;
    register struct USER_ENTRY *current_line;

    numlines = 0;
    numentries = 0;
    current_line = NULL;
    salt[0] = salt[1] = '*';

    while (fgets (buffer, STRINGSIZE, stdin))
    {
	if (!*buffer || isspace (*buffer))
	{
	    continue;
	}
	new_element = Parse (buffer);

	ptr = new_element -> passwd.pw_passwd;
	if (!ptr[0])
	{
	    Log ("Warning! %s (%s in %s) has a NULL password!\n",
		 new_element -> passwd.pw_name,
		 new_element -> passwd.pw_shell,
		 new_element -> filename);
	    continue;
	}
	if (strchr (ptr, '*') ||
	    strchr (ptr, '!') ||
	    strchr (ptr, ' '))
	{
	    Log ("User %s (in %s) has a locked password:- %s\n",
		 new_element -> passwd.pw_name,
		 new_element -> filename,
		 new_element -> passwd.pw_passwd);
	    continue;
	}
#ifdef CRACK_PW_VERIFY
	if (strlen (ptr) < 13)
	{
	    Log ("User %s (in %s) has a short crypted password - ignoring.\n",
		 new_element -> passwd.pw_name,
		 new_element -> filename);
	    continue;
	}
	if (strlen (ptr) > 13)
	{
	    Log ("User %s (in %s) has a long crypted password - truncating.\n",
		 new_element -> passwd.pw_name,
		 new_element -> filename);
	    ptr[13] = '\0';
	}
#endif				/* CRACK_PW_VERIFY */
	numentries++;

	if (ptr[0] == salt[0] && ptr[1] == salt[1])
	{
	    new_element -> across = current_line;
	    current_line = new_element;
	} else
	{
	    if (current_line)
	    {
		current_line -> next = user_root;
	    }
	    user_root = current_line;
	    current_line = new_element;
	    numlines++;
	    salt[0] = ptr[0];
	    salt[1] = ptr[1];
	}
    }

    if (current_line)		/* last one tends to hang about */
    {
	current_line -> next = user_root;
	user_root = current_line;
	numlines++;
    }
    Log ("Loaded %d password entries into %d salted lines.\n",
	 numentries, --numlines);

    return;
}

void
PrintGuess (eptr, guess)
    register struct USER_ENTRY *eptr;
    char *guess;
{
#ifdef CRACK_PRINTOUT		/* prevents (some) appearances in coredumps */
    eptr -> passwd_txt = Clone (guess);	/* essential to FeedBack */
#endif

    Log ("Guessed %s%s (%s in %s) [%s] %s\n",
	 (eptr -> passwd.pw_uid ? "" : "ROOT PASSWORD "),
	 eptr -> passwd.pw_name,
	 eptr -> passwd.pw_shell,
	 eptr -> filename,
#ifdef CRACK_PRINTOUT
	 guess,
#else
	 "<not printed>",
#endif				/* CRACK_PRINTOUT */
#ifdef CRACK_PRINTOUT2
	 eptr -> passwd.pw_passwd
#else
	 ""
#endif				/* CRACK_PRINTOUT2 */
	);
}
/*
 * PASSWORD CRACKING LOW LEVEL FUNCTIONS
 */

int
TryManyUsers (eptr, guess)	/* returns 0 if all done this line */
    register struct USER_ENTRY *eptr;
    char *guess;
{
    register int retval;
    char guess_crypted[STRINGSIZE];

    if (eptr -> done && !eptr -> across)
    {
	return (0);
    }
    strcpy (guess_crypted, crypt (guess, eptr -> passwd.pw_passwd));

    retval = 0;

    while (eptr)
    {
#ifdef CRACK_VERBOSE		/* Really want to fill your filestore ? */
	if (verbose_bool)
	{
	    Log ("Trying '%s' on %s from line %s\n",
		 guess,
		 eptr -> passwd.pw_name,
		 eptr -> filename);
	}
#endif
	if (!strcmp (guess_crypted, eptr -> passwd.pw_passwd))
	{
	    if (!eptr -> done)	/* haven't printed it before */
	    {
		PrintGuess (eptr, guess);
	    }
	    eptr -> done = 1;
	}
	retval += (!eptr -> done);
	eptr = eptr -> across;
    }

    return (retval);
}

int
TryOneUser (eptr, guess)	/* returns non-null on guessed user */
    register struct USER_ENTRY *eptr;
    register char *guess;
{
    if (!guess || !*guess || eptr -> done)
    {
	return (0);
    }
#ifdef CRACK_VERBOSE
    if (verbose_bool)
    {
	Log ("Trying '%s' on %s from %s\n",
	     guess,
	     eptr -> passwd.pw_name,
	     eptr -> filename);
    }
#endif
    if (strcmp (crypt (guess, eptr -> passwd.pw_passwd),
		eptr -> passwd.pw_passwd))
    {
	return (0);
    }
    eptr -> done = 1;
    PrintGuess (eptr, guess);
    return (1);
}
/*
 * TOP LEVEL CRACKING FUNCTION INTERFACE ROUTINE - SINGLE USER ONLY
 */

int
WordTry (entry_ptr, guess)
    register struct USER_ENTRY *entry_ptr;
    register char *guess;
{
    int all_lower;
    int all_upper;
    register char *ptr;
    char guess2[STRINGSIZE];

    if (!guess[1])		/* avoid wasting time on initials */
    {
	return (0);
    }
    if (TryOneUser (entry_ptr, guess) ||
	TryOneUser (entry_ptr, Reverse (guess)))
    {
	return (1);
    }
    all_upper = all_lower = 1;
    ptr = guess;

    for (ptr = guess; *ptr; ptr++)
    {
	if (islower (*ptr))
	{
	    all_upper = 0;
	} else if (isupper (*ptr))
	{
	    all_lower = 0;
	}
    }

    if (!all_lower)
    {
	strcpy (guess2, Lowercase (guess));
	if (TryOneUser (entry_ptr, guess2) ||
	    TryOneUser (entry_ptr, Reverse (guess2)))
	{
	    return (1);
	}
    }
    if (!all_upper)
    {
	strcpy (guess2, Uppercase (guess));
	if (TryOneUser (entry_ptr, guess2) ||
	    TryOneUser (entry_ptr, Reverse (guess2)))
	{
	    return (1);
	}
    }
    return (0);
}
/*
 * END OF PASSWORD GUESSING LAYERS
 */

void
Pass1 ()
{
    int cracked;
    struct USER_ENTRY *head;
    register char *ptr;
    register char *ptr2;
    char junk[STRINGSIZE];
    register struct USER_ENTRY *this;

    Log ("starting pass1 password information\n");

    for (head = user_root; head; head = head -> next)
    {
	for (this = head; this; this = this -> across)
	{
	    /* username */
	    ptr = this -> passwd.pw_name;
	    if (WordTry (this, ptr))
	    {
		continue;
	    }
	    /* usernameusername */
	    strcpy (junk, ptr);
	    strcat (junk, ptr);
	    if (WordTry (this, junk))
	    {
		continue;
	    }
	    /* usernameemanresu */
	    strcpy (junk, ptr);
	    strcat (junk, Reverse (ptr));
	    if (WordTry (this, junk))
	    {
		continue;
	    }
#ifdef CRACK_GECOS
	    /* Gecos information field */
	    cracked = 0;
	    ptr = junk;
	    strcpy (junk, this -> passwd.pw_gecos);
	    if (*ptr == '-')	/* never seen this, but... */
	    {
		ptr++;
	    }
	    if (ptr2 = (char *) strchr (ptr, ';'))	/* trim off junk */
	    {
		*ptr2 = ' ';
	    }
	    if (ptr2 = (char *) strchr (ptr, ','))	/* trim off more junk */
	    {
		*ptr2 = ' ';
	    }
	    for (;;)
	    {
		if (ptr2 = (char *) strchr (ptr, ' '))
		{
		    *(ptr2++) = '\0';
		}
		if (WordTry (this, ptr))
		{
		    cracked++;
		    break;
		}
		if (ptr2)
		{
		    ptr = ptr2;
		    while (*ptr && isspace (*ptr))
		    {
			ptr++;
		    }
		} else
		{
		    break;
		}
	    }

	    if (cracked)
	    {
		continue;
	    }
#endif				/* CRACK_GECOS */
	}
    }
    return;
}

void
Pass2 (dictfile)
    char *dictfile;
{
    FILE *fp;
    register struct USER_ENTRY *head;
    int dictcount;
    struct DICT_ENTRY *dictroot;
    register struct DICT_ENTRY *scratch;
    int pointuser;
    char buffer[STRINGSIZE];

    Log ("Starting pass2 on dictionary %s\n", dictfile);

    if (!(fp = fopen (dictfile, "r")))
    {
	perror (dictfile);
	return;
    }
    dictcount = 0;
    dictroot = (struct DICT_ENTRY *) 0;

    while (fgets (buffer, STRINGSIZE, fp))
    {
	Trim (buffer);
	if (!*buffer)
	{
	    continue;
	}
	scratch = (struct DICT_ENTRY *) malloc (sizeof (struct DICT_ENTRY));
	scratch -> word = Clone (buffer);
	scratch -> next = dictroot;
	dictroot = scratch;
	dictcount++;
    }

    fclose (fp);

    Log ("Loaded %d words from %s\n", dictcount, dictfile);

    if (dictcount <= 0)
    {
	Log ("Oops! Where did I get an empty dictionary from ?\n");
	return;
    }
    head = (struct USER_ENTRY *) 0;	/* See "for" loop below */

    if (recover_bool)		/* We are recovering */
    {
	recover_bool = 0;	/* Switch it off */
	head = user_root;	/* Start of the userlist */
	while (head && rusercount--)	/* For the recover count */
	{
	    head = head -> next;/* Skip to the next user */
	}			/* God this is taking a long time to do */
	if (!head)		/* Legs on the ground, feet at the bottom */
	{
	    Log ("Recover: data changed : looking for %s ran off end of list",
		 rusername);	/* puff, puff, puff, pant pant */
	    Log ("Exiting...\n");
	    exit (0);
	}
	if (verify_recover && strcmp (rusername, head -> passwd.pw_name))
	{
	    Log ("Recover: data changed : looking for %s found %s\n",
		 rusername, head -> passwd.pw_name);
	    Log ("Exiting...\n");	/* almost there */
	    exit (0);
	}
    }
    /* at bloody last */
    pointuser = 0;		/* we havent pointed anyone yet */

    for (head = head ? head : user_root; head; head = head -> next)
    {
	SetPoint (dictfile, pointuser++, head -> passwd.pw_name);
	for (scratch = dictroot; scratch; scratch = scratch -> next)
	{
	    if (!TryManyUsers (head, scratch -> word))
	    {
		break;
	    }
	}
    }

    Log ("Freeing memory from pass of %s\n", dictfile);
    while (dictroot)
    {
	free (dictroot -> word);
	scratch = dictroot -> next;
	free (dictroot);
	dictroot = scratch;
    }
}

void
FeedBack (unguessed)
    int unguessed;
{
    register FILE *fp;
    static char fmt[] = "%s:%s:%s:%s\n";
    register struct USER_ENTRY *head;
    register struct USER_ENTRY *arm;

    Log ("Doing feedback file.\n");

    fp = (FILE *) 0;

    for (head = user_root; head; head = head -> next)
    {
	for (arm = head; arm; arm = arm -> across)
	{
	    if (arm -> done)
	    {
		if (!fp)	/* horrible little hack - I love it */
		{
		    if (!(fp = fopen (feedbackfile, "w")))
		    {
			perror (feedbackfile);
			return;
		    }
		    Log ("Feedback file opened.\n");
		}
		fprintf (fp, fmt, feedback_string, arm -> passwd.pw_passwd,
			 "Y", arm -> passwd_txt);
	    } else
	    {
		if (unguessed)
		{
		    if (!fp)	/* and again !!! heheheheheheh */
		    {
			if (!(fp = fopen (feedbackfile, "w")))
			{
			    perror (feedbackfile);
			    return;
			}
			Log ("Feedback file opened.\n");
		    }
		    /* I think I'm going slightly warped */
		    fprintf (fp, fmt, feedback_string, arm -> passwd.pw_passwd,
			     "N", "");
		}
	    }

	}
    }
    fclose (fp);
    Log ("Done feedback file.\n");
    return;
}

int
main (argc, argv)
    int argc;
    char *argv[];
{
    int i;
    FILE *fp;
    char *file;
    char *crack_out;
    int uerr;
    extern int optind;
    extern char *optarg;
    char rshother[STRINGSIZE];
    int foreground_bool = 0;
    int rshother_bool = 0;
    int feedback_bool = 0;
    int network_bool = 0;

    uerr = 0;
    pid = getpid ();		/* fill this out FIRST THING */

#ifdef CRACK_NETWORK
    if (gethostname (hostname, STRINGSIZE))	/* and this too ! */
    {
	perror ("gethostname");
    }
#endif

    if (argc == 1)
    {
	uerr++;
    }
    while ((i = getopt (argc, argv, "fFNvU:i:r:R:n:")) != EOF)
    {
	switch (i)
	{
	case 'N':
#ifndef CRACK_NETWORK
	    Log ("Called remotely but not compiled with CRACK_NETWORK symbol\n");
	    Log ("Exiting...\n");
	    exit (1);
#else
	    network_bool = 1;
#endif
	    break;
	case 'U':
	    strcpy (rshother, optarg);
	    rshother_bool = 1;
	    break;
	case 'F':
	    feedback_bool = 1;
	    break;
	case 'f':
	    foreground_bool = 1;
	    break;
	case 'v':
	    verbose_bool = 1;
	    break;
	case 'n':
	    nice (atoi (optarg));
	    break;
	case 'R':
	    verify_recover = 1;
	    /* nobreak; */
	case 'r':
	    if (GetPoint (optarg))	/* We are not a recovering machine */
	    {
		exit (0);	/* so, silently die... */
	    }
	    recover_bool = 1;
	    break;
	case 'i':
	    if (!freopen (optarg, "r", stdin))
	    {
		perror (optarg);
		exit (1);
	    }
	    /*
	     * If we unlink this file here it effectively hides it, as well
	     * as preventing the file systems all over the network getting
	     * gummed up with crud. Only delete a tmp file which matches out
	     * standard template. Anything else might be a user's own file
	     * bct@hull.cs
	     */

	    if (strcmp (optarg, "/tmp/pw.", 8))
	    {
		unlink (optarg);
	    }
	    break;
	default:
	case '?':
	    uerr++;
	    break;
	}
    }

    if (optind >= argc)
    {
	uerr++;
    }
    if (uerr)
    {
	fprintf (stderr,
		 "Usage: %s [-NFfv] [-ifile] [-nn] [-<R|r>file] [-Uname] dictfile [...]\n",
		 argv[0]);
	exit (1);
    }
    crack_out = (char *) getenv ("CRACK_OUT");

    if (!crack_out)
    {
	crack_out = ".";
    }
#ifdef CRACK_NETWORK
    sprintf (opfile, "%s/out.%s.%d", crack_out, hostname, pid);
    sprintf (diefile, "%s/die.%s.%d", runtime, hostname, pid);
    sprintf (pointfile, "%s/pnt.%s.%d", runtime, hostname, pid);
    sprintf (feedbackfile, "%s/fbk.%s.%d", runtime, hostname, pid);
#else
    sprintf (opfile, "%s/out.%d", crack_out, pid);
    sprintf (diefile, "%s/die.%d", runtime, pid);
    sprintf (pointfile, "%s/pnt.%d", runtime, pid);
    sprintf (feedbackfile, "%s/fbk.%d", runtime, pid);
#endif				/* CRACK_NETWORK */

    if (!foreground_bool)
    {
	if (!freopen (opfile, "w", stdout))
	{
	    perror ("freopen(stdout)");
	    exit (1);
	}
	if (!freopen (opfile, "a", stderr))
	{
	    perror ("freopen(stderr)");
	    exit (1);
	}
    }
    /*
     * If we are given a "-f" flag from across the network we must still make
     * a die file so the remote machine can still find the right process to
     * kill with a plaster  bct@hull.cs
     */

    if (network_bool || !foreground_bool)
    {
	if (!(fp = fopen (diefile, "w")))
	{
	    perror (diefile);
	    exit (1);
	}
	fprintf (fp, "#!/bin/sh\n");

#ifdef CRACK_NETWORK
	/*
	 * Use the "-n" option on "rsh" to avoid problems of input streams
	 * attached to remote processes. See man page for rsh bct@hull.cs
	 */

	if (rshother_bool)
	{
	    fprintf (fp, "%s %s -n -l %s kill -TERM %d", rcmd, hostname,
		     rshother, pid);
	} else
	{
	    fprintf (fp, "%s %s -n kill -TERM %d", rcmd, hostname, pid);
	}
#else
	fprintf (fp, "kill -TERM %d", pid);
#endif				/* CRACK_NETWORK */
	fprintf (fp, " && rm $0\n");
	fclose (fp);
	chmod (diefile, 0700);
    }
    Log ("Crack: A Sensible Password Cracker v%s by ADE Muffett, 1991\n",
	 version);

#ifdef CRACK_NETWORK
    Log ("We are running on networked machine %s\n", hostname);
#endif

    if (recover_bool)
    {
#ifdef CRACK_NETWORK
	Log ("We are recovering from a crash on %s, PID %d.\n", rhostname, rpid);
#else
	Log ("We are recovering PID %d from a crash.\n", rpid);
#endif
	Log ("From Dictionary %s, User number %d.\n", rdict, rusercount);
	Log ("Username must match %s\n", rusername);
    }
#ifdef FCRYPT
    init_des ();
#endif

    /* Quick, verify that we are sane ! */

    if (strcmp (crypt ("fredfred", "fredfred"), "frxWbx4IRuBBA"))
    {
	Log ("Version of crypt() being used internally is not compatible with standard.\n");
	Log ("This could be due to byte ordering problems - see the comments in Sources/conf.h\n");
	Log ("If there is another reason for this, edit the source to remove this assertion.\n");
	Log ("Terminating...\n");
	exit (0);
    }
    signal (SIGTERM, CatchTERM);

    Log ("Loading...\n");

    LoadData ();

    if (!recover_bool)
    {
	/* We are starting afresh ! Ah, the birds in May ! */
	Pass1 ();
#ifdef CRACK_PRINTOUT
	if (feedback_bool)
	{
	    FeedBack (0);
	}
#endif				/* CRACK_PRINTOUT */
    } else
    {
	/* Some spodulous creep pulled our plug... */
	while ((optind < argc) && strcmp (argv[optind], rdict))
	{
	    optind++;		/* Fly over dicts that we have done */
	}
    }

    for (i = optind; i < argc; i++)
    {
	Pass2 (argv[i]);
#ifdef CRACK_PRINTOUT
	if (feedback_bool)
	{
	    FeedBack (0);
	}
#endif				/* CRACK_PRINTOUT */
    }

    Log ("Finished. Tidying up files...\n");
    if (network_bool || !foreground_bool)
    {
	unlink (diefile);
    }
    unlink (pointfile);

#ifdef CRACK_PRINTOUT
    if (feedback_bool)
    {
	FeedBack (1);
    }
#endif				/* CRACK_PRINTOUT */
    Log ("Done.\n");
    return (0);
}
