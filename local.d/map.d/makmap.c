/*
 *	  mkmap.c:	A program to massage the stuff from the net into
 *	a form `getfrom.c' can understand, in a reasonable time.
*/

#ifndef ORRAW
#define ORRAW	"region"
				/* Name of local Map (kludged to reality) */
#endif
#ifndef UUMAP
#define UUMAP	"map"
				/* Upper triangle of whole net		*/
#endif
#ifndef UURAW
#define UURAW	"world"
				/* Upper triangle of whole net		*/
#endif
#ifndef UUTHR
#define UUTHR	12		/* Threshold for `important' sites	*/
#endif
#ifndef UUTMP
#define UUTMP	"/tmp/uu.tmp"	/* Unsorted upper triangle output	*/
#endif
#define ismail(n) ((strncmp(n,"Mail:",5) == 0) || (strncmp(n,"News:",5) == 0))
#define getlin(fd,s) (fscanf(fd," %[^\n]",s), s[0] ? 1 : 0)

#include <stdio.h>
extern char *malloc();
extern char *strcpy();
extern char *strncpy();

typedef struct s {
  char sitename[8];		/* UUCP name of site			*/
  struct s *next;		/* Link to next site			*/
  int conn;			/* Count of connected sites		*/
} site;

#define hash(n)	(name[0] & 037)
site *sites[32];		/* Hashed list of known sites		*/

FILE *orraw;			/* The input local map file		*/
FILE *uuraw;			/* The input global map file		*/
FILE *uutmp;			/* The unsorted output file		*/
char name1[100];		/* Names as read from map file		*/
char name2[100];		/* Names as read from map file		*/

site *newsite(name)		/* Add new site with given name		*/
char *name;			/* Name of new site			*/
{ site *ns;			/* New site record			*/
  int bucket;			/* Hash bucket for net record		*/

  bucket = hash(name);
  ns = (site *) malloc(sizeof *ns);
  if (ns == NULL) {
    fprintf(stderr,"mkmap: No core for site %s\n",name);
    exit(1);
  }
  strncpy(ns->sitename,name,sizeof ns->sitename);
  ns->sitename[(sizeof ns->sitename)-1] = '\0';
  ns->next = sites[bucket];
  sites[bucket] = ns;
  ns->conn = 0;
  return(ns);
}

site *locate(name)		/* Get site record for given site	*/
char *name;
{ register site *s;		/* 'Current' site			*/

  s = sites[hash(name)];
  while ((s != NULL) && strncmp(s->sitename,name,sizeof s->sitename-1))
    s = s->next;
  if (s == NULL)
    return(newsite(name));
  return(s);
}

connect(n1,n2)
char *n1, *n2;
{ locate(n1)->conn++;		/* Another connection for site n1	*/
  locate(n2)->conn++;		/* Another connection for site n2	*/
}

parsesites(s,n1)
char *s;
char *n1;
{ register char *p;
  register char *q = s;
  do {
    for (p = q; (*p == ' ') || (*p == '\t'); p++);
    if (*p != '\0') {
      for (q = p; (*q != ' ') && (*q != '\t') && (*q != '\0'); q++);
      if (*q == '\0') {
        fprintf(uutmp,"%s %s\n",n1,p);
        fprintf(uutmp,"%s %s\n",p,n1);
        break;
      }
      *q = '\0';
      fprintf(uutmp,"%s %s\n",n1,p);
      fprintf(uutmp,"%s %s\n",p,n1);
      *q++ = ' ';
    }
  } while(*p != '\0');
}

/*
 *	Make map of local region on the temp file.
*/
parse_or()
{ register char *p;		/* Names as found in map file		*/
  register char wasmail;	/* Simplifies loop below		*/

  orraw = fopen(ORRAW,"r");	/* Open local map for input		*/
  if (orraw == NULL) {
    fprintf(stderr,"Cannot open local map\n");
    exit(1);
  }
  uutmp = fopen(UUTMP,"w");	/* Open temp file for output		*/
  if (uutmp == NULL) {
    fprintf(stderr,"Cannot open temp file\n");
    exit(1);
  }
  wasmail = 0;
  while (getlin(orraw,name2)) {
    if (strncmp(name2,"Name:",5) == 0) {
      for (p = name2+5; (*p == ' ') || (*p == '\t'); p++);
      strcpy(name1,p);
    }
    if (ismail(name2) || (wasmail && (name2[0] == '\t'))) {
      parsesites(name2+5,name1);
      wasmail = 1;
    } else
      wasmail = 0;
  }
  if (fclose(orraw) == EOF) {	/* Close local map			*/
    fprintf(stderr,"Cannot close local map\n");
    exit(1);
  }
  if (fclose(uutmp) == EOF) {	/* Close temp file			*/
    fprintf(stderr,"Cannot close temp file\n");
    exit(1);
  }
}

/*
 *	  This routine forks into two processes.  The child reads the
 *	unsorted (and unlinked) version of the local map from parse_or,
 *	while the parent matches the sorted output against UURAW and
 *	writes matches into the sorted version.
 *
 *	  Side Effect: leaves UURAW and UUTMP open on return.
*/
sort_or()
{ static int pipdes[2];		/* File descriptors for the pipe	*/
  static FILE *f;		/* File descriptors for the pipe	*/
  static int tmp;		/* Need fd, not FILE * here		*/

  if (pipe(pipdes) < 0) {	/* Create the pipe			*/
    fprintf(stderr,"Cannot create pipe\n");
    exit(1);
  }
  tmp = open(UUTMP,0);		/* Open temp file for input		*/
  if (unlink(UUTMP) < 0) {	/* Unlink to make room for new one	*/
    fprintf(stderr,"Cannot delete old temp file\n");
    exit(1);
  }
  if (tmp < 0) {
    fprintf(stderr,"Cannot open local map temp file\n");
    exit(1);
  }
  if (fork()) {
    /*
     *	Parent process - put pipe input into UUTMP if it matches
     *	a line of UURAW.
    */
    if (close(tmp) < 0) {	/* Close old temp file			*/
      fprintf(stderr,"Can't close old temp file in parent process\n");
      exit(1);
    }
    if (close(pipdes[1]) < 0) {	/* Close pipe output			*/
      fprintf(stderr,"Can't close pipe output in parent process\n");
      exit(1);
    }
    f = fdopen(pipdes[0],"r");	/* Open pipe as stream			*/
    if (f == NULL) {
      fprintf(stderr,"Can't convert pipe input to stream\n");
      exit(1);
    }
    uuraw = fopen(UURAW,"r");	/* Open global map for input		*/
    if (uuraw == NULL) {
      fprintf(stderr,"Cannot open global map\n");
      exit(1);
    }
    uutmp = fopen(UUTMP,"w");	/* Open new temp file for output	*/
    if (uutmp == NULL) {
      fprintf(stderr,"Cannot open new temp file\n");
      exit(1);
    }

    strcpy(name2,"");		/* Force match low to start		*/
    while (getlin(f,name1)) {
      while (strcmp(name1,name2) > 0)
        if (!getlin(uuraw,name2)) {
          if (fclose(f) == EOF) { /* Done with the pipe			*/
            fprintf(stderr,"Cannot close pipe input\n");
            exit(1);
          }
          return;		/* No more matches - one's empty	*/
        }
      if (strcmp(name1,name2) == 0)
        fprintf(uutmp,"%s\n",name1); /* Got a match - write it		*/
    }
    if (fclose(f) == EOF) {	/* Done with the pipe			*/
      fprintf(stderr,"Cannot close pipe input\n");
      exit(1);
    }
    wait((int *)0);		/* Wait for Child			*/
    return;			/* Job is done				*/
  }
  /*
   *	Child process - put tmp input as stdin, pipe as stdout,
   *	and exec /bin/sort.
  */
  if (close(pipdes[0]) < 0) {	/* Close pipe input			*/
    fprintf(stderr,"Can't close pipe input in child process\n");
    exit(1);
  }
  if (fclose(stdin) == EOF) {	/* Close old stdin			*/
    fprintf(stderr,"Cannot close stdin in child\n");
    exit(1);
  }
  if (dup(tmp) != 0) {		/* Duplicate UUTMP as stdin for child	*/
    fprintf(stderr,"Can't move old temp to stdin in child process\n");
    exit(1);
  }
  if (fclose(stdout) == EOF) {	/* Close old stdout			*/
    fprintf(stderr,"Cannot close stdout in child\n");
    exit(1);
  }
  if (dup(pipdes[1]) != 1) {	/* Open pipe as stdout			*/
    fprintf(stderr,"Can't move pipe to stdout in child process\n");
    exit(1);
  }
  execl("/bin/sort","sort","-u",(char *)NULL);
				/* Turn into /bin/sort			*/
  fprintf(stderr,"exec of /bin/sort failed\n");
  exit(1);
}

/*
 *	  This routine reads the unsorted (and unlinked) version of
 *	the map, and writes the sorted version.  This amounts to
 *	fudging what file are open where, and doing an exec of
 *	/bin/sort.
*/
sort_map()
{ if (fclose(stdin) < 0) {	/* Not planning to read old stdin	*/
    fprintf(stderr,"Cannot close stdin for temp file\n");
    exit(1);
  }
  if (open(UUTMP,0) != 0) {	/* Now UUTMP is stdin			*/
    fprintf(stderr,"Can't move temp file to stdin\n");
    exit(1);
  }
  if (unlink(UUTMP) < 0) {	/* Unlink to make room for new one	*/
    fprintf(stderr,"Cannot delete old temp file\n");
    exit(1);
  }
  if (fclose(stdout) < 0) {	/* Not planning to write old stdout	*/
    fprintf(stderr,"Cannot close stdout for map file\n");
    exit(1);
  }
  if (creat(UUMAP,0644) != 1) {	/* Open map file as stdout		*/
    fprintf(stderr,"Cannot open result map as stdout\n");
    exit(1);
  }
  execl("/bin/sort","sort","-u",(char *)NULL);
				/* Turn into /bin/sort			*/
}

main()
{ site *s;			/* 'Current' site			*/
  register int i;

  for (i = 0; i < 32; i++)
    sites[i] = NULL;		/* Init hash table			*/

  parse_or();			/* Get rid of or.raw garbage		*/
  sort_or();			/* Sort and match with uu.raw		*/

  if (fseek(uuraw,0L,0) < 0) {	/* Rewind network map			*/
    fprintf(stderr,"Cannot rewind global map\n");
    exit(1);
  }
  while (fscanf(uuraw," %s %s",name1,name2) == 2)
    connect(name1,name2);	/* Connect two sites			*/

  if (fseek(uuraw,0L,0) < 0) {	/* Rewind network map			*/
    fprintf(stderr,"Cannot rewind global map for final pass\n");
    exit(1);
  }
  while (fscanf(uuraw," %s %s",name1,name2) == 2) {
    s = locate(name1);		/* Locate the first site		*/
    if (s->conn < UUTHR)	/* Is the first site important?		*/
      continue;			/* Nope, try next edge			*/
    s = locate(name2);		/* Locate the second site		*/
    if (s->conn < UUTHR)	/* Is the second site important?	*/
      continue;			/* Nope, try next edge			*/
    fprintf(uutmp,"%s %s\n",name1,name2);
  }

  if (fclose(uuraw) < 0) {	/* Close network map			*/
    fprintf(stderr,"Cannot close raw global map\n");
    exit(1);
  }
  if (fclose(uutmp) < 0) {	/* Close temp file			*/
    fprintf(stderr,"Cannot close unsorted result tempfile\n");
    exit(1);
  }

  sort_map();			/* Sort temp file into result file	*/
}
