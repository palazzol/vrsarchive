#ifndef lint
static char rcsid[]="$Header: /home/Vince/cvs/local.d/ofiles.d/ofiles.c,v 1.2 1987-12-26 20:19:38 vrs Exp $";
static char sccsid[]="%W% %G% cspencer";
#endif /*lint*/

#ifdef M_XENIX
#define KERNEL	"/xenix"
#else
#define KERNEL	"/unix"
#include <sys/types.h>
#include <sys/signal.h>
#include <sys/immu.h>
#include <sys/region.h>
#include <sys/sysmacros.h>
#include <malloc.h>
#include <nlist.h>
#define ROOTINO S5ROOTINO
#endif
#include <sys/param.h>
#include <sys/dir.h>
#include <sys/file.h>
#include <sys/inode.h>
#include <sys/user.h>
#include <sys/proc.h>
#include <sys/stat.h>
#include <sys/var.h>
#include <pwd.h>
#include <stdio.h>
#include "fstab.h"

#define CDIR	01
#define OFILE	02
#define RDIR	04

extern long lseek();

long eseek();
int nproc;		/* Number of entries in proc table 		*/
int mem;		/* Fd for Physical Memory			*/
int kmem;		/* Fd for kernel DS				*/
int swap;		/* Fd for /dev/swap				*/
short rootdev;		/* Device number of root device			*/
unsigned procbase;	/* Pointer to _proc in kmem			*/
struct var v;		/* Kernel configuartion parameters		*/
int pids_only = 0;	/* If non-zero, only output process ids		*/
char *progname;		/* Name of this program (for messages)		*/

#ifdef M_XENIX
struct nlist nl[] = {
#define X_PROC		0
	{"_proc"},
#define X_V		1
	{"_v"},
#define X_ROOTDEV	2
	{"_rootdev"},
	{ 0 }
};
#else
struct nlist nl[] = {
#define X_PROC		0
	{"proc"},
#define X_V		1
	{"v"},
#define X_ROOTDEV	2
	{"rootdev"},
	{ 0 }
};
#endif


main(argc, argv)
int 	argc;
char	*argv[];
{

	struct inode *i,*getinode();
	struct stat s;
	struct user *u, *getuser();
	struct proc p;
	register int filen, flags, procn;
	register char *filename, *fsname;
	struct fstab *fs, *getfsfile(), *getfsspec();

	progname = argv[0];

	if(argc == 1) {
		fprintf(stderr,"usage: %s [-p] files\n", progname);
		exit(1);
	}

	/* check for -p flag */
	if(strcmp(argv[1],"-p") == 0) {
			pids_only++;
			--argc;
			++argv;
	}

	if((mem = open("/dev/mem", 0)) < 0)
		error("can't open /dev/mem. ");
	if((kmem = open("/dev/kmem", 0)) < 0)
		error("can't open /dev/kmem. ");
	if((swap = open("/dev/swap", 0)) < 0) 
		error("can't open /dev/swap. ");
	getsyms();

	while(--argc) {
		/* assume arg is  a filesystem */
		if((fs = getfsfile(*++argv)) != NULL) {
			fsname = *argv;
			filename = fs->fs_spec;
		}
		/* maybe it's the device name for a filesystem */
		else if ((fs = getfsspec(*argv)) != NULL) {
			filename = *argv;
			fsname = fs->fs_file;
		}
		/* probably a regular file */
		else {
			filename = *argv;
			fsname = "";
		}
		if(stat(filename, &s)) {
			fprintf(stderr,"can't stat %s. ",filename);
			perror("");
			continue;
		}
		if(! pids_only) {
			printf("%s\t%s\n", filename,fsname);
			printf("%-8.8s\tpid\ttype\tcmd\n", "user");
		}
		for(procn = 0; procn < nproc; procn++){
			procslot(procn, &p);
			flags = 0;
			if(p.p_stat == 0 || p.p_stat == SZOMB)
				continue;
			u = getuser(&p);
			if ( u == (struct user *)NULL)
				continue;
			i = getinode(u->u_rdir, "rdir");
			if(check(&s,i))
				flags |= RDIR;

			i = getinode(u->u_cdir, "cdir");
			if(check(&s,i))
				flags |= CDIR;

			for(filen = 0; filen < NOFILE; filen++) {
				struct file f;

				if(u->u_ofile[filen] == NULL)
					continue;

				eseek(kmem,(long)u->u_ofile[filen],0,"file");
				eread(kmem,(char *)&f, sizeof(f), "file");

				if(f.f_count > 0) {
					if (f.f_inode == 0)
						continue;
					i = getinode(f.f_inode, "file");
					if(check(&s,i))
						flags |= OFILE;
				}
			}
			if(flags) gotone(u,&p,flags);
		}
		if(! pids_only)
			printf("\n");
	}
}		

/*
 * print the name of the user owning process "p" and the pid of that process
 */
gotone(u,p,f)
struct user *u;
struct proc *p;
int f;
{
	register struct passwd *pw;
	struct passwd *getpwuid();

	/* only print pids and return */
	if(pids_only) {
			printf("%d ",p->p_pid);
			fflush(stdout);
			return;
	}
	pw = getpwuid(u->u_uid);
	if(pw)
		printf("%-8.8s\t", pw->pw_name );
	else
		printf("(%d)\t", u->u_uid);
	printf("%d\t", p->p_pid);
	if(f & OFILE) putchar('f');	/* proc has a file 		   */	
	else putchar(' ');
	if(f & CDIR)  putchar('p');	/* proc's current dir is on device */
	else putchar(' ');
	if(f & RDIR)  putchar('r');	/* proc's root dir is on device	   */
	else putchar(' ');
	printf("\t");
	printf("%-14.14s", u->u_comm);
	printf("\n");
}

/*
 * is inode "i" on device "s"? returns TRUE or FALSE 
 */
check(s, i)
struct stat *s;
struct inode *i;
{
	if ((s->st_mode & S_IFMT) == S_IFBLK && s->st_rdev == i->i_dev)
			return 1;
	if ((s->st_mode & S_IFMT) == S_IFCHR && s->st_rdev == i->i_dev)
			return 1;
	else if((s->st_dev == i->i_dev) && (s->st_ino == i->i_number))
			return 1;
	else return 0;
}


/* 
 *	getinode - read an inode from from mem at address "addr"
 * 	      return pointer to inode struct. 
 */
struct inode *getinode(ip,s)
struct inode *ip;
char *s;
{
	static struct inode i;

	if (ip == NULL) {
		i.i_dev = rootdev;
		i.i_number = ROOTINO;
	} else {
		eseek(kmem, (long)ip, 0, "inode");
		eread(kmem, (char *)&i, sizeof(i), "inode");
	}
	return &i;
}

/* 
 * get user page for proc "p" from core or swap
 * return pointer to user struct
 */
struct user *getuser(p)
struct proc *p;
{
#ifdef M_XENIX
	int fd;			/* File to read			*/
	long addr;		/* Where to read		*/
	static struct user user;/* Local buffer			*/

	if ((p->p_flag & SLOAD) == 0) {
		fd = swap;
		addr = ((long)BSIZE)*(p->p_addr.p_daddr);
	} else {
		fd = mem;
		addr = mltoa(p->p_addr.p_caddr);
	}
	(void) lseek(fd, addr, 0);
	if (read(fd, (char *)&user, sizeof(user))!=sizeof(user)){
		fprintf(stderr, "error: can't get u structure\n");
		return (struct user *)NULL;
	}
	return(&user);
#else
	/* declaration of space for reading in the ubptbl */
	union {
		char	cbuf[NBPPT];
		pde_t	ptbl[NBPPT/sizeof(pde_t)];
	} upt;
	static char *user = 0;
	static int usize;
	char *cp, *cp1;
	int i;

	if (user == 0) {
		usize = sizeof(struct user)
			+ (v.v_nofiles-1)*sizeof(struct file *);
		user = malloc(usize);
	}
	if (p->p_flag & SLOAD) {
		lseek(kmem, ubptbl((p)), 0);
		read(kmem, upt.cbuf, NBPPT); /* read in the U block pt */
		/* Now read in each page of the U area */
		cp1 = user + usize;
		i = 0;
		for(cp = user; cp < cp1; i++, cp += NBPP) {
			lseek(mem, ctob(upt.ptbl[i].pgm.pg_pfn), 0);
			if (read(mem, cp, cp1-cp > NBPP ? NBPP : cp1-cp) < 0) {
			    fprintf(stderr, "error: can't get u structure\n");
			    return((struct user *)0);
			}
		}
	} else {
		lseek(swap, dtob(((dbd_t *)(&p->p_ubptbl))->dbd_blkno), 0);
		if (read (swap, user, usize) != usize) {
			fprintf(stderr, "error: can't get u structure\n");
			return((struct user *)0);
		}
	}
	return((struct user *)user);
#endif
}

/*
 * read with error checking
 */
eread( fd, p, size, s)
int fd;
char *p;
int size;
char *s;
{
	int n;
	char buf[100];
	if(( n =  read(fd, p, size)) != size){
		sprintf(buf, "read error for %s. ", s);
		error(buf);
	}
	return n;
}

/*
 * seek with error checking
 */
long eseek(fd, off, whence, s)
int fd;
long off;
int whence;
char *s;
{
	long ret;
	char buf[100];

	if(( ret = lseek(fd, off, whence)) != off) {
		sprintf(buf, "seek for %s failed, wanted %o, got %o. ",
			s, off, ret);
		error(buf);
	}
	return ret;
}

/*
 * print mesg "s" followed by system erro msg. exit.
 */
error(s)
char *s;
{
	if (s)
		fprintf(stderr,s);
	perror("");
	exit(1);
}

/*
 * get some symbols from the kernel
 */
getsyms()
{
	register i;

	nlist(KERNEL, nl);

	for(i = 0; i < (sizeof(nl)/sizeof(nl[0]))-1; i++)
		if(nl[i].n_value == 0) {
			fprintf(stderr,"%s: can't nlist for %s.\n",
				KERNEL, nl[i].n_name);
			exit(1);
		}
	procbase = nl[X_PROC].n_value;
	eseek(kmem, (long)nl[X_ROOTDEV].n_value, 0);
	eread(kmem, &rootdev, sizeof(rootdev), "root device number");
	eseek(kmem, (long)nl[X_V].n_value, 0);
	eread(kmem, &v, sizeof(v), "v structure");
	nproc = v.v_proc;
	return;
}
			

/*
 * read proc table entry "n" into buffer "p"
 */
procslot(n, p)
int n;
struct proc *p;
{
	eseek(kmem, procbase + (long)(n * sizeof(struct proc)), 0);
	eread(kmem, (char *)p, sizeof(struct proc), "proc structure");
	return;
}
