#ifndef lint
static char rcsid[] = "$Header: /home/Vince/cvs/local.d/sucko.d/nntp.c,v 1.2 1992-04-07 20:10:07 vrs Exp $";
#endif
/*
 *	This code was shamelessly stolen from XRN by vrs.
*/
#include <signal.h>
#define GROUP_NAME_SIZE	128
#define NO_GROUP    	-1
#define CHAR_OK     	'2'
#define OK_CANPOST		200
#define OK_NOPOST		201
#define ERR_NOGROUP 	411
#define ERR_ACCESS  	502
#define SERVER_FILE		"nntpd"
#include <stdio.h>
#define STREQN(a,b,n)	(strncmp((a),(b),(n)) == 0)
void
mesgPane(str,a1)
char *str, *a1;
{	fprintf(stderr,str,a1);
	fprintf(stderr,"\n");
}
void
ehErrorExitXRN(msg)
{
	perror(msg);
	exit(1);
}
void start_server();

/*
 * Copyright (c) 1988, 1989, The Regents of the University of California.
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted, provided
 * that the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of the University of California not
 * be used in advertising or publicity pertaining to distribution of 
 * the software without specific, written prior permission.  The University
 * of California makes no representations about the suitability of this
 * software for any purpose.  It is provided "as is" without express or
 * implied warranty.
 *
 * THE UNIVERSITY OF CALIFORNIA DISCLAIMS ALL WARRANTIES WITH REGARD TO 
 * THIS SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND 
 * FITNESS, IN NO EVENT SHALL THE UNIVERSITY OF CALIFORNIA BE LIABLE FOR
 * ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER
 * RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF
 * CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN 
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */
#include <X11/Xos.h>
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <ctype.h>
#include <assert.h>
#include <sys/param.h>

#define BUFFER_SIZE 1024
#define MESSAGE_SIZE 1024


#ifdef notdef
#if defined(AF_DECnet) && defined(ultrix)
# ifndef DECNET
#  define DECNET
# endif
#endif
#endif notdef

#ifdef DECNET
#include <netdnet/dn.h>
#include <netdnet/dnetdb.h>
#endif

#ifdef BSD_BFUNCS
#define memset(_Str_, _Chr_, _Len_) bzero(_Str_, _Len_)
#define memcpy(_To_, _From_, _Len_) bcopy(_From_, _To_, _Len_)
#endif

FILE	*ser_rd_fp = NULL;
FILE	*ser_wr_fp = NULL;

/*
 * getinfofromfile	Get a string from a named file
 *			Handle white space and comments.
 *
 *	Parameters:	"file" is the name of the file to read.
 *
 *	Returns:	Pointer to static data area containing the
 *			first non-ws/comment line in the file.
 *			NULL on error (or lack of entry in file).
 *
 *	Side effects:	None.
 */
char *
getinfofromfile(file)
char	*file;
{
	register FILE	*fp;
	register char	*cp;
	static char	buf[256];
	char		*getenv();

	if (file == NULL)
		return (NULL);

	fp = fopen(file, "r");
	if (fp == NULL)
		return (NULL);

	while (fgets(buf, sizeof (buf), fp) != NULL) {
		if (*buf == '\n' || *buf == '#')
			continue;
		cp = index(buf, '\n');
		if (cp)
			*cp = '\0';
		(void) fclose(fp);
		return (buf);
	}

	(void) fclose(fp);
	return (NULL);			 /* No entry */
}


/*
 * getserverbyfile	Get the name of a server from a named file.
 *			Handle white space and comments.
 *			Use NNTPSERVER environment variable if set.
 *
 *	Parameters:	"file" is the name of the file to read.
 *
 *	Returns:	Pointer to static data area containing the
 *			first non-ws/comment line in the file.
 *			NULL on error (or lack of entry in file).
 *
 *	Side effects:	None.
 */
char *
getserverbyfile(file)
char	*file;
{
    char *cp;
    extern char *getenv(), *getinfofromfile();
    static char	buf[256];

    if (cp = getenv("NNTPSERVER")) {
	(void) strcpy(buf, cp);
    } else {
	cp = getinfofromfile(file);
	if (cp == NULL) {
	    return(NULL);
	} else {
	    (void) strcpy(buf, cp);
	}
    }
    return (buf);
}

/*
 * server_init  Get a connection to the remote news server.
 *
 *	Parameters:	"machine" is the machine to connect to.
 *
 *	Returns:	-1 on error
 *			server's initial response code on success.
 *
 *	Side effects:	Connects to server.
 *			"ser_rd_fp" and "ser_wr_fp" are fp's
 *			for reading and writing to server.
 */

server_init(machine)
char	*machine;
{
	int	sockt_rd, sockt_wr;
	char	line[256];
#ifdef DECNET
	char	*cp;
#endif /* DECNET */

#ifdef DECNET
	cp = index(machine, ':');

	if (cp && cp[1] == ':') {
		*cp = '\0';
		sockt_rd = get_dnet_socket(machine);
	} else
		sockt_rd = get_tcp_socket(machine);
#else  /* DECNET */
	sockt_rd = get_tcp_socket(machine);
#endif /* DECNET */

	if (sockt_rd < 0)
		return (-1);

	/*
	 * Now we'll make file pointers (i.e., buffered I/O) out of
	 * the socket file descriptor.  Note that we can't just
	 * open a fp for reading and writing -- we have to open
	 * up two separate fp's, one for reading, one for writing.
	 */
	if ((ser_rd_fp = fdopen(sockt_rd, "r")) == NULL) {
		perror("server_init: fdopen #1");
		return (-1);
	}

	sockt_wr = dup(sockt_rd);
	if ((ser_wr_fp = fdopen(sockt_wr, "w")) == NULL) {
		perror("server_init: fdopen #2");
		ser_rd_fp = NULL;		/* from above */
		return (-1);
	}

	/* Now get the server's signon message */

	(void) get_server(line, sizeof(line));
	return (atoi(line));
}

/*
 * get_tcp_socket -- get us a socket connected to the news server.
 *
 *	Parameters:	"machine" is the machine the server is running on.
 *
 *	Returns:	Socket connected to the news server if
 *			all is ok, else -1 on error.
 *
 *	Side effects:	Connects to server.
 *
 *	Errors:		Printed via perror.
 */
get_tcp_socket(machine)
char	*machine;
{
	int	s;
#ifdef h_addr
	int x = 0;
	register char **cp;
#endif h_addr
	struct	sockaddr_in sin;
	struct	servent *getservbyname(), *sp;
	struct	hostent *gethostbyname(), *hp;

	if ((sp = getservbyname("nntp", "tcp")) ==  NULL) {
		(void) fprintf(stderr, "nntp/tcp: Unknown service.\n");
		return (-1);
	}

	if ((hp = gethostbyname(machine)) == NULL) {
		(void) fprintf(stderr, "%s: Unknown host.\n", machine);
		return (-1);
	}

	(void) memset((char *) &sin, 0, sizeof(sin));
	sin.sin_family = hp->h_addrtype;
	sin.sin_port = sp->s_port;

	/*
	 * The following is kinda gross.  The name server under 4.3
	 * returns a list of addresses, each of which should be tried
	 * in turn if the previous one fails.  However, 4.2 hostent
	 * structure doesn't have this list of addresses.
	 * Under 4.3, h_addr is a #define to h_addr_list[0].
	 * We use this to figure out whether to include the NS specific
	 * code...
	 */

#ifdef	h_addr

	/* get a socket and initiate connection -- use multiple addresses */

	for (cp = hp->h_addr_list; cp && *cp; cp++) {
		s = socket(hp->h_addrtype, SOCK_STREAM, 0);
		if (s < 0) {
			(void) perror("socket");
			return (-1);
		}
		(void) memcpy((char *) &sin.sin_addr, *cp, hp->h_length);
		
		if (x < 0) {
		    (void) fprintf(stderr, "trying %s\n", inet_ntoa(sin.sin_addr));
		}
		x = connect(s, (struct sockaddr *)&sin, sizeof (sin));
		if (x == 0)
			break;
                (void) fprintf(stderr, "connection to %s: ", inet_ntoa(sin.sin_addr));
		(void) perror("");
		(void) close(s);
	}
	if (x < 0) {
		(void) fprintf(stderr, "giving up...\n");
		return (-1);
	}
#else	/* no name server */

	if ((s = socket(AF_INET, SOCK_STREAM, 0)) < 0) { /* Get the socket */
		(void) perror("socket");
		return (-1);
	}

	/* And then connect */

	(void) memcpy((char *) &sin.sin_addr, hp->h_addr, hp->h_length);
	if (connect(s, (struct sockaddr *) &sin, sizeof(sin)) < 0) {
		(void) perror("connect");
		(void) close(s);
		return (-1);
	}

#endif

	return (s);
}

#if defined(DECNET) || defined(VMS)
/*
 * get_dnet_socket -- get us a socket connected to the news server.
 *
 *	Parameters:	"machine" is the machine the server is running on.
 *
 *	Returns:	Socket connected to the news server if
 *			all is ok, else -1 on error.
 *
 *	Side effects:	Connects to server.
 *
 *	Errors:		Printed via nerror.
 */

get_dnet_socket(machine)
char	*machine;
{	int	s, area, node;
	struct	sockaddr_dn sdn;
	struct	nodeent *getnodebyname(), *np;

	(void) memset((char *) &sdn, 0, sizeof(sdn));

	switch (s = sscanf( machine, "%d%*[.]%d", &area, &node )) {
		case 1: 
			node = area;
			area = 0;
		case 2: 
			node += area*1024;
			sdn.sdn_add.a_len = 2;
			sdn.sdn_family = AF_DECnet;
			sdn.sdn_add.a_addr[0] = node % 256;
			sdn.sdn_add.a_addr[1] = node / 256;
			break;
		default:
			if ((np = getnodebyname(machine)) == NULL) {
			    (void) fprintf(stderr, 
				    "%s: Unknown host.\n", machine);
			    return (-1);
			} else {
			    (void) memcpy((char *) sdn.sdn_add.a_addr, 
					  np->n_addr, 
					  np->n_length);
			    sdn.sdn_add.a_len = np->n_length;
			    sdn.sdn_family = np->n_addrtype;
			}
			break;
	}
	sdn.sdn_objnum = 0;
	sdn.sdn_flags = 0;
	sdn.sdn_objnamel = strlen("NNTP");
	(void) memcpy(&sdn.sdn_objname[0], "NNTP", sdn.sdn_objnamel);

	if ((s = socket(AF_DECnet, SOCK_SEQPACKET, 0)) < 0) {
		nerror("socket");
		return (-1);
	}

	/* And then connect */

	if (connect(s, (struct sockaddr *) &sdn, sizeof(sdn)) < 0) {
		nerror("connect");
		close(s);
		return (-1);
	}
	return (s);
}
#endif /* DECNET or VMS */

/*
 * handle_server_response
 *
 *	Print some informative messages based on the server's initial
 *	response code.  This is here so inews, rn, etc. can share
 *	the code.
 *
 *	Parameters:	"response" is the response code which the
 *			server sent us, presumably from "server_init",
 *			above.
 *			"server" is the news server we got the
 *			response code from.
 *
 *	Returns:	-1 if the error is fatal (and we should exit).
 *			0 otherwise.
 *
 *	Side effects:	None.
 */
handle_server_response(response, server)
int	response;
char	*server;
{
    switch (response) {
	case OK_NOPOST:		/* fall through */
    		fprintf(stderr,
	"NOTE: This machine does not have permission to post articles.\n");
		fprintf(stderr,
	"      Please don't waste your time trying.\n\n");

	case OK_CANPOST:
		return (0);

	case ERR_ACCESS:
		fprintf(stderr,
   "This machine does not have permission to use the %s news server.\n",
		server);
		return (-1);

	default:
		fprintf(stderr,"Unexpected response code from %s news server: %d\n",
			server, response);
		return (-1);
    }
	/*NOTREACHED*/
}


/*
 * put_server -- send a line of text to the server, terminating it
 * with CR and LF, as per ARPA standard.
 *
 *	Parameters:	"string" is the string to be sent to the
 *			server.
 *
 *	Returns:	Nothing.
 *
 *	Side effects:	Talks to the server.
 *
 *	Note:		This routine flushes the buffer each time
 *			it is called.  For large transmissions
 *			(i.e., posting news) don't use it.  Instead,
 *			do the fprintf's yourself, and then a final
 *			fflush.
 */
void
put_server(string)
char *string;
{
#ifdef DEBUG
	(void) fprintf(stderr, ">>> %s\n", string);
#endif
	(void) fprintf(ser_wr_fp, "%s\r\n", string);
	(void) fflush(ser_wr_fp);
}


void
sigalrm()
{
}
/*
 * get_server -- get a line of text from the server.  Strips
 * CR's and LF's.
 *
 *	Parameters:	"string" has the buffer space for the
 *			line received.
 *			"size" is the size of the buffer.
 *
 *	Returns:	-1 on error, 0 otherwise.
 *
 *	Side effects:	Talks to server, changes contents of "string".
 */
get_server(string, size)
char	*string;
int	size;
{
	void (*alrm)();
	register char *cp;

	alrm = signal(SIGALRM, sigalrm);
	alarm(10);
	if (fgets(string, size, ser_rd_fp) == NULL)
		return (-1);
	alarm(0);
	(void) signal(SIGALRM, alrm);

	if ((cp = index(string, '\r')) != NULL)
		*cp = '\0';
	else if ((cp = index(string, '\n')) != NULL)
		*cp = '\0';
#ifdef DEBUG
	(void) fprintf(stderr, "<<< %s\n", string);
#endif

	return (0);
}


/*
 * close_server -- close the connection to the server, after sending
 *		the "quit" command.
 *
 *	Parameters:	None.
 *
 *	Returns:	Nothing.
 *
 *	Side effects:	Closes the connection with the server.
 *			You can't use "put_server" or "get_server"
 *			after this routine is called.
 */
void
close_server()
{
	char	ser_line[256];

	if (ser_wr_fp == NULL || ser_rd_fp == NULL)
		return;

	put_server("QUIT");
	(void) get_server(ser_line, sizeof(ser_line));

	(void) fclose(ser_wr_fp);
	(void) fclose(ser_rd_fp);
}

/*
 *	Routines for communicating with the NNTP remote news server
*/
int ServerDown = 1;
static char mybuf[MESSAGE_SIZE+100] = 
	"The news server is not responding correctly, aborting\n";

static void
get_data_from_server(str, size)
char *str;     /* string for message to be copied into */
int size;      /* size of string                       */
/*
 * get data from the server (active file, article)
 *
 *  on error, sets 'ServerDown'
 *
 *   returns: void
 */
{
    if (get_server(str, size) < 0) {
	ServerDown = 1;
    } else {
	ServerDown = 0;
    }
    return;
}


static void
check_time_out(command, response, size)
char *command;  /* command to resend           */
char *response; /* response from the command   */
int size;       /* size of the response buffer */
{
    /*
     * try to recover from a timeout
     *
     *   this assumes that the timeout message stays the same
     *   since the error number (503) is used for more than just
     *   timeout
     *
     *   Message is:
     *     503 Timeout ...
     */

	if (ServerDown || STREQN(response, "503 Timeout", 11)) {

		mesgPane("Lost connection to the NNTP server, attempting to reconnect");
		start_server((char *)0);
		mesgPane("Reconnected to the NNTP server");
		
		/*
		 * if it was an ARTICLE for XHDR command, then you must get the
		 * server into the right state (GROUP mode), so resend the last
		 * group command
		 */
		if (STREQN(command, "ARTICLE", 7) || STREQN(command, "XHDR", 4)) {
			if (getgroup((char *)NULL) == NO_GROUP) {
			return;
			}
		}
		
		put_server(command);
		get_data_from_server(response, size);
	}
    
    return;
}


/*
 * retrieve article number 'artnumber' in the current group, update structure
 *
 *   returns:  filename that the article is stored in or NULL if
 *             the article is not avaiable
 *
 */
static FILE *
getarticle(artnumber)
long artnumber;  /* number of article in the current group to retrieve */
{
    char command[MESSAGE_SIZE], message[MESSAGE_SIZE], *msg;
    FILE *articlefp;
    char *filename, *ptr;
#ifdef aiws
    char dummy[MAXPATH];
#else
    char dummy[MAXPATHLEN];
#endif /* aiws */
    char field[BUFFER_SIZE];
    int byteCount = 0;

    /* send ARTICLE */
    (void) sprintf(command, "ARTICLE %ld", artnumber);
    put_server(command);
    get_data_from_server(message, sizeof(message));

    check_time_out(command, message, sizeof(message));

    if (*message != CHAR_OK) {
		/* can't get article */
		return(NULL);
    }

    (void) sprintf(dummy, "/tmp/xrn%ld-XXXXXX", artnumber);
    if ((filename = mktemp(dummy)) == 0) {
		mesgPane("can not create a file name for the article file");
		return(0);
    }

    if ((articlefp = fopen(filename, "w+")) == NULL) {
		mesgPane("can not open a temporary file for the article, `%s' may not be writable", "/tmp");
		return(0);
    }
	(void) unlink(filename);

	for (;;) {
		get_data_from_server(message, sizeof(message));

		/* the article is ended by a '.' on a line by itself */
		if ((message[0] == '.') && (message[1] == '\0')) {
			/* check for a bogus message */
			if (byteCount == 0) {
			(void) fclose(articlefp);
			(void) unlink(filename);
			return(0);
			}
			break;
		}

		msg = &message[0];

		if (*msg == '.') {
			msg++;
		}

		if (*msg != '\0') {
			/* strip leading ^H */
			while (*msg == '\b') {
			msg++;
			}
			/* strip '<character>^H' */
			for (ptr = index(msg + 1, '\b'); ptr != 0; ptr = index(ptr, '\b')) {
			if (ptr - 1 < msg) {
				/* too many backspaces, kill all leading back spaces */
				while (*ptr == '\b') {
					(void) strcpy(ptr, ptr + 1);
				ptr++;
				}
				break;
			}
			(void) strcpy(ptr - 1, ptr + 1);
			ptr--;
			}

			/* handle ^L (poorly) */
			if (*msg == '\014') {
			int i;
			for (i = 0; i < 60; i++) {
				(void) putc('\n', articlefp);
			}
			byteCount += 60;
			continue;
			}
			(void) fputs(msg, articlefp);
		}
		(void) putc('\n', articlefp);
		byteCount += strlen(msg) + 1;
	}

    (void) fseek(articlefp, 0L, 0);
    return(articlefp);
}


/*
 * enter a new group and get its statistics (and update the structure)
 *   allocate an array for the articles and process the .newsrc article
 *   info for this group
 *
 *   returns: NO_GROUP on failure, 0 on success
 *
 */
int
getgroup(name, first, last, number)
char *name;     /* group name                 */
{
    char command[MESSAGE_SIZE], message[MESSAGE_SIZE];
    char group[GROUP_NAME_SIZE];
    long code, num, count, frst, lst;
    static char lastGroup[GROUP_NAME_SIZE];

    if (name == 0) {
		name = lastGroup;
    } else {
		(void) strcpy(lastGroup, name);
    }

    (void) sprintf(command, "GROUP %s", name);
    put_server(command);
    get_data_from_server(message, sizeof(message));

    check_time_out(command, message, sizeof(message));
    
    if (*message != CHAR_OK) {
	if (atoi(message) != ERR_NOGROUP) {

		(void) strcat(mybuf, "\tRequest was: ");
		(void) strcat(mybuf, command);
		(void) strcat(mybuf, "\n");
		(void) strcat(mybuf, "\tFailing response was: ");
		(void) strcat(mybuf, message);
		ehErrorExitXRN(mybuf);
	}
	mesgPane("Can't get the group, looks like it was deleted out from under us");
	
	/* remove the group from active use ??? */
	
	return(0);
    }

    /* break up the message */
    count = sscanf(message, "%ld %ld %ld %ld %s", &code, &num, &frst, &lst, group);
    assert(count == 5);

    return(1);
}


/*
 * get a list of all active newsgroups and create a structure for each one
 *
 *   returns: void
 */
char *
nntp_active()
{
    char command[MESSAGE_SIZE], message[MESSAGE_SIZE], group[GROUP_NAME_SIZE];
#ifdef aiws
    char dummy[MAXPATH];
#else
    char dummy[MAXPATHLEN];
#endif /* aiws */
    FILE *activefp;
    char *filename;
    char type;
    struct newsgroup *newsgroup;
    long first, last, art;
    char *ptr;
    int count = 0;

	start_server((char *)0);
    (void) strcpy(command, "LIST");
    put_server(command);
    get_data_from_server(message, sizeof(message));

    check_time_out(command, message, sizeof(message));
    
	if (*message != CHAR_OK) {
		(void) strcat(mybuf, "\tRequest was: ");
		(void) strcat(mybuf, command);
		(void) strcat(mybuf, "\n");
		(void) strcat(mybuf, "\tFailing response was: ");
		(void) strcat(mybuf, message);
		ehErrorExitXRN(mybuf);
	}

    (void) sprintf(dummy, "/tmp/clp%ld-XXXXXX", 0);
    if ((filename = mktemp(dummy)) == 0) {
		mesgPane("can not create a file name for the active file");
		return(0);
    }

    if ((activefp = fopen(filename, "w+")) == NULL) {
		mesgPane("can not open a temporary file for the active file, `%s' may not be writable", "/tmp");
		return(0);
    }
	(void) unlink(filename);

	for (;;) {
		get_data_from_server(message, sizeof(message));
		
		/* the list is ended by a '.' at the beginning of a line */
		if (*message == '.') {
			break;
		}

		/* server returns: group last first y/m */

		if (sscanf(message, "%s %ld %ld %c", group, &last, &first, &type) != 4) {
			mesgPane("bogus active file entry, skipping\n%s", message);
			continue;
		}

		if (first == 0) {
			first = 1;
		}
		fprintf(activefp, "%s\n", message);
	}
	fseek(activefp, 0L, 0);
	return(activefp);
}



/*
 * initiate a connection to the news server
 *
 * nntpserver is the name of an alternate server (use the default if NULL)
 *
 * the server eventually used is remembered, so if this function is called
 * again (for restarting after a timeout), it will use it.
 *
 *   returns: void
 *
 */
void
start_server()
{
    static char *server = 0;   /* for restarting */
    int response, connected;
    extern void sleep();

    if (server == 0) {
	    if ((server = getserverbyfile(SERVER_FILE)) == NULL) {
			mesgPane("can't get the name of the news server from `%s'",
					   SERVER_FILE);
			mesgPane("either fix this file, or put NNTPSERVER in your environment");
			ehErrorExitXRN("");
	    }
	}

	do {
		if ((response = server_init(server)) < 0) {
			connected = 0;
			mesgPane("Failed to reconnect to the NNTP server (server_init), sleeping...");
			sleep(60);
			continue;
		}
		if (handle_server_response(response, server) < 0) {
			connected = 0;
			close_server();
			mesgPane("Failed to reconnect to the NNTP server (handle_response), sleeping...");
			sleep(60);
			continue;
		}
		connected = 1;
	} while (!connected);
    
    return;
}
    

/*
 *	Pretend to open a file in /usr/spool/news.
 *	Actually opens a copy in /tmp.
*/
FILE *
nntp_open(filename)
char *filename;
{	register char *p, *ap;

	filename += strlen("/usr/spool/news/");
	/* Undo ngdir() effect, find article number */
	ap = NULL;
	for (p = filename; *p; p++) {
		if (*p == '/') {
			*p = '.';
			ap = p; 
		}
	}
	if (ap == NULL) {
		fprintf(stderr, "nntp_open: bogus newsgroup '%s'\n", filename);
		return(NULL);
	}
	*ap++ = '\0';
	if (!getgroup(filename)) {
		fprintf(stderr, "nntp_open: bogus newsgroup '%s'\n", filename);
		return(NULL);
	}
	return(getarticle(atol(ap)));
}
