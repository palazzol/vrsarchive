#include <sys/param.h>
#include <sys/tty.h>
#include <nlist.h>
#include <stdio.h>

extern long lseek();

struct nlist nl[] = {
	{ "_dmf_tty" },		/* The tty structures for dmf driver	*/
#define	X_DMF_TTY	0
	{ "_ndmf" },		/* The number of dmf controllers	*/
#define	X_N_DMF		1
	{ "_dmsoft" },		/* Software carrier flags		*/
#define	X_USRPT		2
	{ "" },
};
struct tty dmf_tty;		/* Copy a dmf tty structure		*/
long tp;			/* pointer to dmf tty structure		*/
int ndmf;			/* Stores number of dmf controllers	*/
int fd;				/* File descriptor for /dev/kmem	*/

getac(p)			/* Get a character from the kernel	*/
char **p;
{ char *cl, c;			/* Pointer to last character in kernel	*/

  do {
    lseek(fd,tp,0);
    read(fd,(char *)&dmf_tty,sizeof dmf_tty); /* Get tty structure	*/
    if (dmf_tty.t_rawq.c_cf == NULL) {
      *p = NULL;
      return('\n');
    }
    cl = dmf_tty.t_rawq.c_cl;
  } while (*p == cl);		/* Loop while no character to get	*/
  if (((long)(*p) & CROUND) == 0) { /* Hit end of cblock?		*/
    *p = (char *) (((long)*p) - CROUND - 1); /* Back up to front	*/
    lseek(fd,(long)*p,0);
    read(fd,(char *)p,sizeof (*p)); /* Get address of next cblock	*/
    *p += sizeof (char *);	/* Skip pointer portion			*/
  }
  lseek(fd,(long)(*p),0);
  read(fd,&c,sizeof c);		/* Get a character of buffered input	*/
  (*p)++;			/* Bump pointer for next time		*/
  return(c);			/* Return to caller			*/
}

main(argc,argv)
int  argc;
char *argv[];
{ int which_tty;		/* Minor number desired			*/
  char *cp;			/* Pointer to character			*/

  if (argc < 2) {
    fprintf(stderr,"usage: %s <minor#>\n",argv[0]);
    exit(1);
  }

  nlist("/vmunix",nl);		/* Get the magic pointers into kmem	*/
  fd = open("/dev/kmem",0);	/* Open kernel memory			*/
  if (fd < 0) {
    fprintf(stderr,"Cannot open /dev/kmem\n");
    exit(1);
  }

  which_tty = atoi(argv[1]);	/* Get and validate minor number	*/
  lseek(fd,(long)nl[X_N_DMF].n_value,0); /* How many dmf lines?		*/
  read(fd,(char *)&ndmf,sizeof ndmf);
  if (which_tty > (ndmf-1)) {
    fprintf(stderr,"There are only %d dmf lines on the system\n",ndmf);
    exit(1);
  }
  
  tp = nl[X_DMF_TTY].n_value+which_tty*(sizeof dmf_tty);
  while (1) {
    do {
      lseek(fd,tp,0);
      read(fd,(char *)&dmf_tty,sizeof dmf_tty); /* Get tty structure	*/
      cp = dmf_tty.t_rawq.c_cf;	/* Point to first character		*/
    } while (cp == NULL);
    while (cp != NULL) {	/* Dump contents of clist		*/
      putchar(getac(&cp));
      fflush(stdout);
    }
  }
}
