/*
** Mail address data structures
*/

#undef	NEED_BZERO
#define NSUBDOM		20      	/* # of subdomain names in domain */

typedef unsigned int MALLOCT;		/* Parameter to malloc		*/

/*
**  An address.
*/
typedef struct _addr {
    struct _addr	*next;		/* next address in list		*/
    struct _dom		*route;		/* route icl. destination domain */
    struct _dom		*destdom;	/* destination domain		*/
    char		*localp;	/* RFC local part		*/
    char		*name;		/* Comment name			*/
    char		*group;		/* Group (List) phrase		*/
    char		*comment;	/* () comment phrase		*/
    char		*error;		/* error text if not NULL	*/
} Addr;


/*
**  A domain.
*/
typedef struct _dom {
    struct _dom		*next;		/* next domain (f.i. in route)	*/
    char		*sub[NSUBDOM];	/* subdomain strins		*/
    char		**top;		/* toplevel domain		*/
} Dom;


extern Addr	*newAddr();		/* Create a new address		*/
extern Dom	*newDom();		/* Create a new domain		*/
extern Addr	*adrlist;
extern Addr	*errlist;
extern char	*malloc();
extern char	*strcpy();
extern char	*strncpy();
extern char	*strcat();

/* SHUT UP! */
#define Strcpy		(void)strcpy
#define Strncpy		(void)strncpy
#define Strcat		(void)strcat
#define Sprintf		(void)sprintf
