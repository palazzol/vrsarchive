#ifdef SCCS
static char *sccsid = "@(#)talk.c	1.1	4/5/85";
static char *cpyrid = "@(#)Copyright (C) 1985 by D Bell";
#endif

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <utmp.h>
#include "war.h"

#define	NAMELEN	(sizeof(ud.ut_name))		/* length of name field */
#define	LINELEN	(sizeof(ud.ut_line))		/* length of line field */
#define	DEVLEN	(sizeof(dev) - 1)		/* length of device string */
#define	FULLEN	(LINELEN + DEVLEN + 1)		/* length of full tty name */
#define	INFO	20				/* number of messages */

char	utmp[] = "/etc/utmp";	/* logged in user file */
char	dev[] = "/dev/";	/* prefix for tty names */
int	sd;			/* descriptor for socket */
int	serverflag;		/* nonzero if we are the server */
struct	sockaddr addr;		/* name of our socket */
int	infocount;		/* number of queued messages */
struct	info	info[INFO];	/* messages queued for sending */


/*
 * Set the socket address used for connecting to the other player.
 * We find the other player by his user name (and his tty name if necessary).
 * The address is generated using the device numbers of our two terminals.
 * Exits if an error is detected.  If successful, saves the socket address
 * and remembers whether we are to be the server or the client.
 */
findplayer(user, tty)
	register char	*user;		/* other player's user name */
	register char	*tty;		/* other player's tty name, or NULL */
{
	register FILE	*fd;		/* file pointer */
	register char	*shortname;	/* short tty name */
	register int	count;		/* times user is logged in */
	int	sawme;			/* true if saw myself */
	int	dev1;			/* device of first tty */
	int	dev2;			/* device of second tty */
	struct	stat	sb;		/* file status */
	struct	utmp	ud;		/* utmp entry */
	char	fullname[FULLEN];	/* full tty name */

	if (fstat(STDERR, &sb)) {	/* we aren't on a tty */
		perror("stderr");
		exit(1);
	}
	if ((sb.st_mode & S_IFMT) != S_IFCHR) {
		fprintf(stderr, "stderr: not a character device\n");
		exit(1);
	}
	dev1 = sb.st_rdev;
	fd = fopen(utmp, "r");
	if (fd == NULL) {
		perror(utmp);
		exit(1);
	}
	strcpy(fullname, dev);
	shortname = &fullname[DEVLEN];
	shortname[LINELEN] = '\0';
	count = 0;
	sawme = 0;
	while (fread(&ud, sizeof(ud), 1, fd)) {
		if (ud.ut_name[0] == '\0') continue;
		if (strncmp(user, ud.ut_name, NAMELEN)) continue;
		strncpy(shortname, ud.ut_line, LINELEN);
		if (stat(fullname, &sb) < 0) continue;
		if ((sb.st_mode & S_IFMT) != S_IFCHR) continue;
		if (sb.st_rdev == dev1) {	/* see if this is myself */
			sawme = 1;
			continue;
		}
		dev2 = sb.st_rdev;
		count++;
		if ((tty == NULL) || strcmp(tty, shortname)) continue;
		count = 1;			/* found user on tty */
		break;
	}
	fclose(fd);
	if (count <= 0) {
		if (sawme)
			fprintf(stderr, "you are not logged in elsewhere\n");
		else
			fprintf(stderr, "%s is not logged in\n", user);
		exit(1);
	}
	if (tty && strcmp(tty, shortname)) {
		fprintf(stderr, "%s is not logged in on %s\n", user, tty);
		exit(1);
	}
	if (count > 1) {
		fprintf(stderr, "%s is logged in more than once\n", user);
		exit(1);
	}
	if (dev1 > dev2) {		/* order the devices */
		count = dev1;
		dev1 = dev2;
		dev2 = count;
		serverflag = 1;
	}
	sprintf(addr.sa_data, "WAR.%d.%d", dev1, dev2);
}



/*
 * Initialize to talk to the other player.
 */
talkinit()
{
	if (chdir(GAMEDIR)) {		/* go to right place */
		perror(GAMEDIR);
		exit(1);
	}
	dpymove(-1, 1);
	dpystr("Waiting for other player...");
	dpyhome();
	dpyupdate();
	if (serverflag) {		/* we are the server */
		serverinit();
		dpymove(-1, 1);
		dpyclrwindow();
		sendboard();
	} else {			/* we are the client */
		clientinit();
		dpymove(-1, 1);
		dpyclrwindow();
		readinfo();
		sendboard();
	}
	beep();
	playing = 1;
	newstat = 1;
}


/*
 * Here if we are the server process, to create the binding.
 * Wait for a connection from the client.
 */
serverinit()
{
	register int	s;		/* socket descriptor */
	int	dummylen;		/* dummy length */
	struct	sockaddr dummyaddr;	/* name of our socket */

	s = socket(AF_UNIX, SOCK_STREAM, PF_UNSPEC);	/* create socket */
	if (s < 0) {
		perror("socket");
		exit(1);
	}
	unlink(addr.sa_data);			/* remove binding name */
	if (bind(s, &addr, sizeof(addr)) < 0) {		/* declare ourself */
		perror("bind");
		exit(1);
	}
	if (listen(s, 1) < 0) {			/* allow one connection */
		perror("listen");
		exit(1);
	}
	dummylen = sizeof(dummyaddr);
	sd = accept(s, &dummyaddr, &dummylen);
	if (sd < 0) {
		perror("accept");
		exit(1);
	}
	close(s);
}


/*
 * Here if we are the client process, to connect to the binded address.
 * We just continuously try to connect to the address.
 */
clientinit()
{
	sd = socket(AF_UNIX, SOCK_STREAM, PF_UNSPEC); /* create socket */
	if (sd < 0) {
		perror("socket");
		exit(1);
	}
	while (connect(sd, &addr, sizeof(addr)) < 0) {
		if ((errno != ECONNREFUSED) && (errno != ENOENT)) {
			perror("connect");
			exit(1);
		}
		sleep(1);
	}
}


/*
 * Send the initial board layout
 */
sendboard()
{
	register struct	cell *cc;	/* current cell within row */
	register struct	object *obj;	/* object at this location */

	for (cc = homecell; cc; cc = cc->c_next) {
		obj = cc->c_obj;
		if ((obj == NULL) || (obj->o_side != myside)) continue;
		sendinfo('p', cc->c_row, cc->c_col, obj->o_id);
	}
	sendinfo('r', 0, 0, 0);
}


/*
 * Send an information message to the other player.
 * Successive message are buffered up and sent together in one write.
 * All pending messages are flushed when an 'r' message type is used.
 */
sendinfo(type, row, col, id)
	register int	type;		/* message type */
	unsigned int	row;
	unsigned int	col;
{
	register struct	info	*ip;	/* pointer to current info block */

	if ((row >= ROWS) || (col >= COLS)) panic("badsend");
	ip = &info[infocount++];
	ip->i_type = type;
	ip->i_row = row;
	ip->i_col = col;
	ip->i_id = id;
	if ((type != 'r') && (infocount < INFO))
		return;			/* wait till later */
	type = (infocount * sizeof(struct info));
	if (write(sd, info, type) != type) {
		dpyclose();
		fprintf(stderr, "other player quit\n");
		exit(0);
	}
	infocount = 0;
}


/*
 * Read and process commands from the other player until we are released
 * with a ready command.  We transform incoming id's and row numbers.
 */
readinfo()
{
	register int	row;		/* row of interest */
	register int	col;		/* column of interest */
	register int	id;		/* id of object */
	register struct	cell *cc;	/* current cell */
	register struct	object *obj;	/* current object */
	struct	info	info;		/* command being read */

	while (1) {
		id = read(sd, &info, sizeof(info));
		if (id <= 0) {
			dpyclose();
			fprintf(stderr, "other player quit\n");
			exit(0);
		}
		if (id != sizeof(info)) panic("short read from socket");
		id = info.i_id ^ 1;		/* toggle id */
		row = (ROWS - 1) - info.i_row;	/* and row number */
		col = info.i_col;
		if ((row >= ROWS) || (col >= COLS)) panic("bad coordinates");
		cc = &board[row][col];
		obj = cc->c_obj;

		switch (info.i_type) {

		case 'b':		/* wall got blasted */
			if ((obj == NULL) || ((obj->o_flags & F_WALL) == 0))
				panic("blasting non-wall");
			removeobject(row, col);
			break;

		case 'h':		/* man got hit */
			if ((obj == NULL) || (obj->o_side != myside))
				panic("missed me");
			hitobject(cc->c_row, cc->c_col);
			break;

		case 'p':		/* object got placed or moved */
			if (obj) panic("nested objects");
			obj = findid(id);
			if (obj->o_side == myside) panic("read my own object"); 
			placeobject(obj, row, col);
			break;

		case 'r':		/* ready for other player */
			return;

		default:
			panic("bad command read from opponent\n");
		}
	}
}
