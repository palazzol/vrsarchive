#include <sys/types.h>
#include <ctype.h>
#include <nlist.h>
#include <fcntl.h>
#include <termio.h>
#include <sys/tty.h>

extern int errno;
extern char *sys_errlist[];

struct nlist mynlist[] = {
	"serial_tty",	(long)0, (short)0, (unsigned short)0, (char)0, (char)0,
	"",				(long)0, (short)0, (unsigned short)0, (char)0, (char)0
};

main(argc, argv)
	int argc;
	char *argv[];
{
	int c;
	char *sysname = "/unix";
	int verbose=0;
	int ttyminor=0;
	extern char *optarg;
	extern int optind;
	extern long strtol();
	struct tty ts;

	/* "-m minornum" specifies which tty to check */
	/* "-n filename" says use filename for symbols (default "/unix") */
	/* "-t ttytype" specifies type of tty */
	/* "-v" says verbose -- tells you what's it's doing */
	while ((c = getopt(argc, argv, "m:n:t:v")) != -1) {
		switch (c) {
		case 'm':
			ttyminor = atoi(optarg);
			break;
		case 'n':
			sysname = optarg;
			break;
		case 't':
			mynlist[0].n_name = optarg;
			break;
		case 'v':
			verbose++;
			break;
		}
	}
	if (InitTtyInfo(sysname, verbose) == -1) {
		printf("%s: InitTtyInfo failed\n", argv[0]);
		exit(1);
	}
	if (GetTtyInfo(ttyminor, verbose, &ts) == -1) {
		printf("%s: GetTtyInfo failed, ttyminor=%d\n", ttyminor);
		exit(1);
	}
	printf("counts: rawq=%d, canq=%d, outq=%d, tbuf=%d, rbuf=%d\n",
		ts.t_rawq.c_cc, ts.t_canq.c_cc, ts.t_outq.c_cc,
		ts.t_tbuf.c_count, ts.t_rbuf.c_count);
	PrintStat(ts.t_state, ts.t_iflag, ts.t_oflag, ts.t_cflag, ts.t_lflag);
}

/*	InitTtyInfo() -- initialize terminal info getting routines */
/*		Does an nlist(3) to get the table base addresses */
/*		Expects "mynlist" to be the info to get */
InitTtyInfo(sysname, verbose)
	char *sysname;	/* name of system to get symbols from */
	int verbose;	/* TRUE if to print progress info */
{
	struct nlist *np;

	if (nlist(sysname, &mynlist[0]) == -1) {
		printf("InitTtyInfo: nlist failed: %s\n", sys_errlist[errno]);
		return(-1);
	}
	if (verbose) {
		for (np=(&mynlist[0]); strlen(np->n_name)!=0; ++np) {
			printf(" nlist: sym=%s, val=%lx\n", np->n_name, np->n_value);
		}
	}
	return(0);
}

/*	GetTtyInfo() -- get the kernel structures for a particular terminal */
/*		You must call InitTtyInfo() first to get "mynlist" initialized */
/*		Opens "/dev/kmem" internally. */
GetTtyInfo(ttyminor, verbose, ts)
	int ttyminor;	/* minor number of tty to read */
	int verbose;	/* TRUE if to print progress info */
	struct tty *ts;	/* tty structure to return */
{
	int unit;
	int fp;
	struct tty *tp;

	if ((fp = open("/dev/kmem", O_RDONLY)) == -1) {
		printf("GetTtyInfo: cannot open/dev/kmem\n");
		return(-1);
	}
	unit = ttyminor;
	tp = ((struct tty *)mynlist[0].n_value)+unit;
	if (verbose) {
		printf("GetTtyInfo: unit=%d, tp=%x\n", unit, tp);
	}
	if (readfile(fp, ts, tp, sizeof(struct tty)) == -1) {
		if (verbose) {
			printf("GetTtyInfo: cannot read struct tty info, minor=%d\n",
					ttyminor);
		}
		return(-1);
	}
	close(fp);
	return(0);
}

/*	getvalue() -- get a value from kernel memory */
/*		Usually used after the nlist(3) call to get values (sizeof(int) long) */
/*		from the kernel. */
getvalue(fp, addr)
int fp;	/* open(2) descriptor for open file */
long addr;/* address to get int value from */
{
	int temp;

	if (readfile(fp, &temp, addr, sizeof(int)) == -1) {
		printf("getvalue: value get fail\n");
		return(0);
	}
	return(temp);
}

/*  readfile() -- read specified number of bytes from kernel image */
readfile(fp, buf, addr, len)
int fp;	/* open(2) descriptor for open file */
char *buf;	/* buffer to read into */
long addr;	/* address to read from kernel image */
int len;	/* number of bytes to read from kernel into buffer */
{
	if (lseek(fp, addr, 0) == -1) {
		printf("readfile: lseek failed, errno='%s'\n", sys_errlist[errno]);
		printf("readfile: fp=%d, &buf=%x, addr=%x, len=%d\n",
				fp, buf, addr, len);
		return(-1);
	}
	if (read(fp, buf, len) != len) {
		printf("readfile: read failed, errno='%s'\n", sys_errlist[errno]);
		printf("readfile: fp=%d, &buf=%x, addr=%x, len=%d\n", 
				fp, buf, addr, len);
		return(-1);
	}
	return(0);
}

PrintStat(state, iflag, oflag, cflag, lflag)
	unsigned short state, iflag, oflag, cflag, lflag;
{
	printf("t_state == %07o", state);
	if (state & TTXOFF)
		printf(" TTXOFF");
	if (state & TTXON)
		printf(" TTXON");
	if (state & TTIOW)
		printf(" TTIOW");
	if (state & RTO)
		printf(" RTO");
#ifdef CLESC
	if (state & CLESC)
		printf(" CLESC");
#endif
#ifdef ESC
	if (state & ESC)
		printf(" ESC");
#endif
	if (state & TACT)
		printf(" TACT");
	if (state & EXTPROC)
		printf(" EXTPROC");
	if (state & TTSTOP)
		printf(" TTSTOP");
	if (state & IASLP)
		printf(" IASLP");
	if (state & OASLP)
		printf(" OASLP");
	if (state & BUSY)
		printf(" BUSY");
	if (state & CARR_ON)
		printf(" CARR_ON");
	if (state & TBLOCK)
		printf(" TBLOCK");
	if (state & ISOPEN)
		printf(" ISOPEN");
	if (state & WOPEN)
		printf(" WOPEN");
	if (state & TIMEOUT)
		printf(" TIMEOUT");
	printf("\n");
	printf("t_iflag == %07o", iflag);
	if (iflag & IXOFF)
		printf(" IXOFF");
	if (iflag & IXANY)
		printf(" IXANY");
	if (iflag & IXON)
		printf(" IXON");
	if (iflag & IUCLC)
		printf(" IUCLC");
	if (iflag & ICRNL)
		printf(" ICRNL");
	if (iflag & IGNCR)
		printf(" IGNCR");
	if (iflag & INLCR)
		printf(" INLCR");
	if (iflag & ISTRIP)
		printf(" ISTRIP");
	if (iflag & INPCK)
		printf(" INPCK");
	if (iflag & PARMRK)
		printf(" PARMRK");
	if (iflag & IGNPAR)
		printf(" IGNPAR");
	if (iflag & BRKINT)
		printf(" BRKINT");
	if (iflag & IGNBRK)
		printf(" IGNBRK");
	printf("\n");
	printf("t_oflag == %07o", oflag);
	switch (oflag&FFDLY) {
		case FF0: printf(" FF0"); break;
		case FF1: printf(" FF1"); break;
	}
	switch (oflag&VTDLY) {
		case VT0: printf(" VT0"); break;
		case VT1: printf(" VT1"); break;
	}
	switch (oflag&BSDLY) {
		case BS0: printf(" BS0"); break;
		case BS1: printf(" BS1"); break;
	}
	switch (oflag&TABDLY) {
		case TAB0: printf(" TAB0"); break;
		case TAB1: printf(" TAB1"); break;
		case TAB2: printf(" TAB2"); break;
		case TAB3: printf(" TAB3"); break;
	}
	switch (oflag&CRDLY) {
		case CR0: printf(" CR0"); break;
		case CR1: printf(" CR1"); break;
		case CR2: printf(" CR2"); break;
		case CR3: printf(" CR3"); break;
	}
	switch (oflag&NLDLY) {
		case NL0: printf(" NL0"); break;
		case NL1: printf(" NL1"); break;
	}
	if (oflag & OFDEL)
		printf(" OFDEL");
	if (oflag & OFILL)
		printf(" OFILL");
	if (oflag & ONLRET)
		printf(" ONLRET");
	if (oflag & ONOCR)
		printf(" ONOCR");
	if (oflag & OCRNL)
		printf(" OCRNL");
	if (oflag & ONLCR)
		printf(" ONLCR");
	if (oflag & OLCUC)
		printf(" OLCUC");
	if (oflag & OPOST)
		printf(" OPOST");
	printf("\n");
	printf("t_cflag == %07o", cflag);
	if (cflag & CLOCAL)
		printf(" CLOCAL");
	if (cflag & HUPCL)
		printf(" HUPCL");
	if (cflag & PARODD)
		printf(" PARODD");
	if (cflag & PARENB)
		printf(" PARENB");
	if (cflag & CREAD)
		printf(" CREAD");
	if (cflag & CSTOPB)
		printf(" CSTOPB");
	switch (cflag & CSIZE) {
		case CS8: printf(" CS8"); break;
		case CS7: printf(" CS7"); break;
		case CS6: printf(" CS6"); break;
		case CS5: printf(" CS5"); break;
	}
	switch (cflag & CBAUD) {
		case B38400: printf(" B38400"); break;
		case B19200: printf(" B19200"); break;
		case B9600:  printf(" B9600"); break;
		case B4800:  printf(" B4800"); break;
		case B2400:  printf(" B2400"); break;
		case B1800:  printf(" B1800"); break;
		case B1200:  printf(" B1200"); break;
		case B600:   printf(" B600"); break;
		case B300:   printf(" B300"); break;
		case B200:   printf(" B200"); break;
		case B150:   printf(" B150"); break;
		case B134:   printf(" B134"); break;
		case B110:   printf(" B110"); break;
		case B75:    printf(" B75"); break;
		case B50:    printf(" B50"); break;
		case B0:     printf(" B0"); break;
		default:     printf(" B???"); break;
	}
	printf("\n");
	printf("t_lflag == %07o", lflag);
#ifdef XCLUDE
	if (lflag & XCLUDE)
		printf(" XCLUDE");
#endif
	if (lflag & NOFLSH)
		printf(" NOFLSH");
	if (lflag & ECHONL)
		printf(" ECHONL");
	if (lflag & ECHOK)
		printf(" ECHOK");
	if (lflag & ECHOE)
		printf(" ECHOE");
	if (lflag & ECHO)
		printf(" ECHO");
	if (lflag & XCASE)
		printf(" XCASE");
	if (lflag & ICANON)
		printf(" ICANON");
	if (lflag & ISIG)
		printf(" ISIG");
	printf("\n");
}
