/*
 *	  Yet another in a long tradition of 'how do I get there from here'
 *	programs for uucp paths.
*/

#ifndef MAP
#define MAP	"/usr/local/lib/uucp/map"
				/* Known connectivity of sites		*/
#endif
#define MAXINT	(((unsigned)-1)>>1)

#include <stdio.h>
extern char *malloc();

typedef struct s {
  char sitename[8];		/* UUCP name of site			*/
  struct l *parent;		/* Breadth first traversal mark		*/
  struct s *next;		/* Link to next site			*/
  struct l *conn;		/* List of connected sites		*/
  int depth;			/* Depth in breadth first tree		*/
} site_type;

typedef struct l {		/* List of connected sites		*/
  struct s *site;		/* Link to connected site		*/
  struct l *con;		/* List of connected sites		*/
} list;

typedef struct q {		/* Queue of sites			*/
  struct s *site;		/* Link to site				*/
  struct q *f;			/* Forward link				*/
  struct q *b;			/* Backward link			*/
} queue;

#define hash(n)	(name[0] & 037)
site_type *sites[32];		/* Hashed list of known sites		*/

int addtolist(l,s)		/* Add site s to list l			*/
list **l;
site_type *s;
{ list *t;			/* Temporary list element		*/

  for (t = *l; t != NULL; t = t->con)
    if (t->site == s)
      return(0);		/* Already there - nothing to do	*/
  t = (list *) malloc(sizeof *t);
  if (t == NULL)
    return(1);			/* Return failure			*/
  t->site = s;			/* Make new list element name site s	*/
  t->con  = *l;			/* Splice into list l			*/
  *l = t;
  return(0);
}

site_type *newsite(name)	/* Add new site with given name		*/
char *name;			/* Name of new site			*/
{ site_type *ns;		/* New site record			*/
  int bucket;			/* Hash bucket for net record		*/

  bucket = hash(name);
  ns = (site_type *) malloc(sizeof *ns);
  if (ns == NULL) {
    fprintf(stderr,"getfrom: No core to create site %s\n",name);
    exit(2);
  }
  strncpy(ns->sitename,name,sizeof ns->sitename);
  ns->sitename[(sizeof ns->sitename)-1] = '\0';
  ns->next = sites[bucket];
  sites[bucket] = ns;
  ns->parent = NULL;
  ns->conn = NULL;
  ns->depth = MAXINT;
  return(ns);
}

site_type *locate(name)		/* Get site record for given site	*/
char *name;
{ register site_type *s;	/* 'Current' site			*/

  s = sites[hash(name)];
  while ((s != NULL) && strncmp(s->sitename,name,sizeof s->sitename-1))
    s = s->next;
  if (s == NULL)
    return(newsite(name));
  return(s);
}

connect(n1,n2)			/* Connect sites n1 and n2		*/
char *n1, *n2;
{ site_type *s1, *s2;

  s1 = locate(n1);		/* Locate first site			*/
  s2 = locate(n2);		/* Locate second site			*/
  if (addtolist(&s2->conn,s1)) {/* Connect s1 to s2			*/
    fprintf(stderr,"Cannot connect %s to %s\n",s1,s2);
    exit(2);
  }
  if (addtolist(&s1->conn,s2)) {/* Connect s2 to s1			*/
    fprintf(stderr,"Cannot connect %s to %s\n",s2,s1);
    exit(2);
  }
}

dumppaths(s,t)			/* Print ancestry of a site		*/
site_type *s;			/* Site to dump about			*/
char *t;			/* Tail to print after each string	*/
{ list *l;			/* Parent list for site s		*/
  char *n;			/* New tail string			*/

  if (s->parent == NULL) {
    printf("%s\n",t+1);		/* Tail has full path name		*/
    return;
  }
  n = malloc((unsigned)(strlen(s->sitename)+strlen(t)+2));
  if (n == NULL) {
    fprintf(stderr,"getfrom: No core to format path\n");
    exit(2);
  }
  strcpy(n,"!");
  strcat(n,s->sitename);
  strcat(n,t);			/* Form !$site$tail			*/
  for (l = s->parent; l != NULL; l = l->con)
    dumppaths(l->site,n);	/* Dump predecessors			*/
  free(n);			/* Free memory for parent recursion	*/
}

main(argc,argv)
int argc;
char *argv[];
{ static int  i;
  static FILE *map;		/* The map file				*/
  static char name1[100];	/* Names as read from map file		*/
  static char name2[100];	/* Names as read from map file		*/
  static site_type *s;		/* The starting site			*/
  static site_type *d;		/* The destination site			*/
  static list *l;		/* The 'current' site			*/
  static queue *h, *t;		/* Head and tail of work queue		*/

  if (argc != 3) {
    fprintf(stderr,"Usage: %s <fromsite> <tosite>\n",argv[0]);
    exit(2);
  }

  for (i = 0; i < 32; i++)
    sites[i] = NULL;		/* Init hash table			*/

  map = fopen(MAP,"r");		/* Open network map			*/
  while (fscanf(map," %s %s",name1,name2) == 2)
    connect(name1,name2);	/* Connect two sites			*/
  fclose(map);			/* Close network map			*/

  s = locate(argv[1]);		/* Locate starting site			*/
  s->parent = NULL;		/* Mark it as seen			*/
  s->depth = 0;			/* Seen at depth zero			*/
  d = locate(argv[2]);		/* Locate destination site		*/
  h = t = (queue *) malloc(sizeof *h); /* Create work queue		*/
  if (h == NULL) {
    fprintf(stderr,"getfrom: No core to create work queue\n");
    exit(2);
  }
  h->site = s;			/* Put starting site in			*/
  h->f = h->b = NULL;		/* Init forward and backward links	*/
  
/*
 *	  At this point 'h' points to a queue record for the site to be
 *	explored.  As sites are found to be reachable from h->site, they
 *	are added to the queue and the tail pointer 't' is adjusted.  When
 *	all such sites have been added the 'h' is adjusted and storage
 *	is freed. If the destination is located, the path is dumped.
*/
  for (; h != NULL; free((char *)h), h = h->f)
    { if (h->site->depth >= d->depth)
        continue;		/* Don't go deeper than shortest path	*/
      l = h->site->conn;	/* Get list of connected sites		*/
      for (l = h->site->conn; l != NULL; l = l->con)
        { if (l->site->depth < h->site->depth + 1)
            continue;		/* Already found a better way		*/
          if (l->site->depth > h->site->depth + 1) {
            l->site->parent = NULL; /* Found better lineage		*/
            l->site->depth = h->site->depth + 1;
          }
          if (addtolist(&l->site->parent,h->site)) {
            fprintf(stderr,
                    "Cannot record %s as parent of %s\n",
                    h->site,
                    l->site);
            exit(2);
          }
          t->f = (queue *) malloc(sizeof (queue));
          if (t->f == NULL) {
            fprintf(stderr,
                    "getfrom: No core to add %s to work queue\n",
                    l->site->sitename);
            exit(2);
          }
          t->f->b = t;		/* Fix back link			*/
          t = t->f;		/* Add to work queue			*/
          t->f = NULL;		/* Finish initialization		*/
          t->site = l->site;
        }
    }
  if (d->parent == NULL)
    exit(1);			/* Could not find path			*/
  dumppaths(d,"");		/* Print path to destination		*/
  exit(0);
}
