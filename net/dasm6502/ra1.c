/* Object File Reverse Assembler
 * Author:   Mark R. Rinfret
 * Date:     05/13/85
 * Filename: ra1.c
 */
#include "ra.h"

/* external data */
extern struct label *xdefhd,*xdeftl;  /* head,tail of xdef label list */
extern struct label *xrefhd,*xreftl;  /* head,tail pointers for xref list */
extern FILE lst;             /* listing file */
extern FILE obj;              /* object file */
extern unsigned bias,codesz,relsz,xrefsz,datasz;
extern char *codebuf;        /* code buffer     */
extern unsigned prompt;      /* interactive mode flag */
extern unsigned *relbuf;     /* relocation data */

/* global data */
struct label *newlbl;        /* new label pointer */

/****************/
main (argc, argv)
unsigned argc;
char **argv;
{

   lst = stdout;             /* default is standard output */
   bias = 0x1800;            /* starting address for shell programs */
   prompt = 0;               /* non-interactive mode */

   while (argc>2) {
     if (!strcmp(*++argv,"-l")) { /* listing option */
       if ((lst=openfile(*++argv,"w"))==NULL || ferror()) {
         printf("can't open listing file, %s\n",*argv);
         exit();
       }
       argc -= 2;
     }
     else if (!strcmp(*argv,"-p")) { /* interactive option */
       prompt = 1;
       argc--;
     }
     else if (!strcmp(*argv,"-a")) { /* change bias */
       sscanf(*++argv,"%x",&bias);
       argc -= 2;
     }
     else {
       printf("--bad option: %s\n",*argv);
       exit();
     }
   } /* end while */

   if (argc<2) usage();

   if ((obj = openfile(*++argv, "r")) == NULL || ferror()) {
     printf ("can't open object file, %s\n", *argv);
   }
   else {
     reverse ();
     fclose (obj);
   }
}

/* 
 * Display correct program usage.
 *
 */
usage()
{
  printf("usage: ra [options] object file\n");
  printf("options:\n"); 
  printf("  -p (enable format prompting)\n");
  printf("  -l listfile (output to listfile)\n");
  printf("  -a address (align at hex address)\n");
  exit();
}

reverse()
{
  xdefhd = NULL;
  xdeftl = NULL;
  xrefhd = NULL;
  xreftl = NULL;
  getcode(); /* input code segment  */
  getrel();  /* get relocation data */
  getxdef(); /* get external id's   */
  getxref();      /* get external ref's  */
  rvrs();        /* do reverse assembly */
}

/* Input code segment */
getcode()
{
  char *malloc();

  codesz = getw(obj);
#ifdef DEBUG
  fprintf(lst,"Code Size: %5d(D) / %x(X)\n",codesz,codesz);
#endif
  codebuf = malloc(codesz);
  fread(codebuf,1,codesz,obj);
}

/* 
 * Input relocation information and perform relocation.
 *
 */

getrel()
{
  char *malloc();
  char *cp; /* temp code pointer */
  unsigned ad,cnt1,cnt2;
  unsigned *rp;
   

  relsz = getw(obj);
  relbuf = (int *) malloc(relsz*2);
  fread(relbuf,2,relsz,obj);
#ifdef DEBUG
  fprintf(lst,"%d relocation entries\n",relsz);
  cnt1 = relsz;
  cnt2 = 0;
  rp = relbuf;
  while (cnt1--) {
    fprintf(lst,"%04x  ",*rp++);
    if (++cnt2 == 8) {
      fputc('\n',lst);
      cnt2=0;
    }
  }
  fprintf(lst,"\n;--------------------\n");
#endif

/* Relocate the code according to 'bias' */
  rp = relbuf;
  cnt1 = relsz;
  while (cnt1--) {
/*
 * Compute pointer into code buffer.  It is important to note that the
 * relocation entry always assumes that an opcode is present (3 byte code
 * entry).  Thus, relocation entries which modify character string pointers
 * appear to be one less than they should be.
 *
 */
    cp = codebuf + *rp++ + 1;
    ad = *cp;                 /* get low byte of address */
    ad = ad | (*(cp+1) << 8); /* 'or' with upper byte */
    ad = ad + bias;           /* relocate */    
    *cp = ad;                 /* put back lower */
    *(cp+1) = (ad >> 8);      /* put back upper */      
  }
}

/* Input external identifiers */
getxdef()
{
  char *malloc();

  unsigned count,i,ofst;
  char id[MAXID];

  count = getw(obj); /* get count of id's */
#ifdef DEBUG
  fprintf(lst,"%d external definitions\n",count);
#endif
  while (count--) {
    if (!(newlbl = (struct label *) malloc(sizeof(struct label)))) {
      fprintf(lst,"Out of memory!");
      abort();
    }
    getid(id,obj); /* input identifier */
    newlbl->name = malloc(strlen(id)+1);
    strcpy(newlbl->name,id);
    newlbl->flag = getc(obj);
    ofst = getw(obj);
    if (newlbl->flag) ofst += bias; /* relocate symbol to start address */
    newlbl->offset = ofst;
    newlbl->next = NULL;

#ifdef DEBUG
    fprintf(lst,"%-12s  ",newlbl->name);
    fprintf(lst,"%c  ",(newlbl->flag ? 'R' : 'A'));
    fprintf(lst,"%04x\n",newlbl->offset);
#endif

    if (xdefhd==NULL) { /* first entry */
      xdefhd = newlbl;
      xdeftl = newlbl;
    }
    else { /* add to end of list */
      xdeftl->next = newlbl;
      xdeftl = newlbl;
    }
  }
}

/*
 * Input external references
 *
 */

getxref()
{
  struct label *malloc();
  unsigned count;
  char id[MAXID];
  struct label *newxref;

  count=getw(obj); /* get number of xrefs */
#ifdef DEBUG
  fprintf(lst,"%d external references.\n\n",count);
#endif
  while (count--) {
    getid(id,obj); /* input identifier */
    newxref=malloc(sizeof(struct label));
    newxref->name = malloc(strlen(id));
    strcpy(newxref->name,id);
    newxref->next = NULL;
    newxref->flag = getw(obj);
    newxref->offset = getw(obj)+bias;

    if (xrefhd==NULL) { /* first entry */
      xrefhd = newxref;
      xreftl = newxref;
    }
    else {
      xreftl->next = newxref;
      xreftl = newxref;
    } 
  }
}
/*
 * Input identifier from file, replacing 'kludge' characters
 *
 */

getid(s,f)
char *s;
FILE f;
{
  char *s1;
  char c;

  s1 = s; /* save copy of pointer */
  while (*s++ = getc(f))
    ;

  if (!isalpha(*s1)) { /* kludge label? */
    *s1++ = '.';
    if (*s1) { /* more than 1 character? */
      *s1++ = '.';
    }
  }
  while (c=*s1) {    /* check rest of label */
    if (!isprint(c)) *s1 += ('a'-1); /* make printable character */
    s1++;
  }
}
