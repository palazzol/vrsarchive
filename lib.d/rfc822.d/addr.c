#include <stdio.h>
#include "addr.h"



#ifdef	NEED_BZERO
/*
**  I forget what bzero is called for SystemV, so...
*/
bzero(p, i)
    register char	*p;
    register int	 i;
{
    while (--i >= 0)
	*p++ = '\0';
}
#endif	/* NEED_BZERO */


/*
**  Create and initialize a new address.
*/
Addr *
newAddr()
{
    register Addr	*ap;

    if ((ap = (Addr *)malloc((MALLOCT)sizeof *ap)) == NULL) {
	perror("Addr alloc");
	exit(1);
    }
    bzero((char *)ap, sizeof *ap);
    return(ap);
}


/*
**  Append addresslist "addr" to addresslist "head".
*/
appAddr(head, addr)
    Addr		**head;
    Addr		 *addr;
{
    register Addr	*ap;

    if (*head) {
	for (ap = *head; ap->next; ap = ap->next)
	    ;
	ap->next = addr;
    }
    else
	*head = addr;
}



/*
**  Create and initialize a new domain.
*/
Dom *
newDom()
{
    register Dom	*dp;

    if ((dp = (Dom *)malloc((MALLOCT)sizeof *dp)) == NULL) {
	perror("Dom alloc");
	exit(1);
    }
    bzero((char *)dp, sizeof *dp);
    dp->top = dp->sub;
    return(dp);
}


/*
**  Append domainlist "dom" to domainlist "head".
*/
appDom(head, dom)
    Dom			**head;
    Dom			*dom;
{
    register Dom	*dp;

    if (*head) {
	for (dp = *head; dp->next; dp = dp->next)
	    ;
	dp->next = dom;
    }
    else
    *head = dom;
}


/*
**  Prepend domainlist "dom" before domainlist "head".
*/
prepDom(head, dom)
    Dom			**head;
    Dom			 *dom;
{
    register Dom	 *dp;

    for (dp = dom; dp->next; dp = dp->next)
	;
    dp->next = *head;
    *head = dom;
}
