/* signals.c 1.1	(c) copyright 1986 (Dan Heller) */

#include "mush.h"
/* #define MYQSORT */

static int order;
static jmp_buf sortbuf;

sort(argc, argv, list)
register int argc;
register char *argv[], list[];
{
    int status_cmp(), author_cmp(), date_cmp(), subject_cmp(), subj_with_re();
    int (*oldint)(), (*oldquit)();
    int (*how)() = status_cmp;
    int n, offset = -1, range = 0;

    order = 1;

    while (argc && *++argv) {
	n = 0;
	while (argv[0][n])
	    switch(argv[0][n++]) {
		case '-': order = -1;
		when 'd': how = date_cmp;
		when 'a': how = author_cmp;
		when 's': how = subject_cmp;
		when 'R': how = subj_with_re;
		when 'S': how = status_cmp;
		otherwise: return help(0, "sort_help", cmd_help);
	    }
    }
    if (msg_cnt <= 1) {
	print("Not enough messages to sort.\n");
	return -1;
    }
    on_intr();

    if (list && ison(glob_flags, IS_PIPE)) {
	for (n = 0; n < msg_cnt; n++)
	    if (msg_bit(list, n)) {
		if (offset < 0)
		    offset = n;
		range++;
	    } else if (offset >= 0)
		break;
    } else
	offset = 0, range = msg_cnt;

    if (range < 2)
	print("Range not broad enough to sort anything\n");
    else {
	Debug("Sorting %d messages starting at message %d\n", range, offset+1);

	if (setjmp(sortbuf) == 0)
	    qsort((char *)&msg[offset], range, sizeof (struct msg), how);
	else
	    print("WARNING: Sorting interrupted: unpredictable order.\n");
	turnon(glob_flags, DO_UPDATE);
    }
    off_intr();
    return -1;
}

#ifdef MYQSORT
qsort(base, len, siz, compar)
register struct msg *base;
int (*compar)();
{
     register int i, swapping;
     struct msg temp;

     do  {
	 swapping = 0;
	 for (i = 0; i < len-1; ++i) {
	     if (compar(base+i, base+i+1) > 0) {
		 temp = base[i];
		 base[i] = base[i+1];
		 base[i+1] = temp;
		 swapping = 1;
	     }
	 }
     } while (swapping);
}
#endif MYSORT

status_cmp(msg1, msg2)
register struct msg *msg1, *msg2;
{
    if (ison(glob_flags, WAS_INTR))
	longjmp(sortbuf, 1);
    if (msg1 < msg || msg2 < msg) {
	wprint("sort botch trying to sort %d and %d\n", msg1-msg, msg2-msg);
	return 0;
    }
    if (msg1->m_flags == msg2->m_flags)
        return 0;
    if (ison(msg1->m_flags, DELETE) && isoff(msg2->m_flags, DELETE))
	return order;
    if (isoff(msg1->m_flags, DELETE) && ison(msg2->m_flags, DELETE))
	return -order;
    if (isoff(msg1->m_flags, OLD) && ison(msg2->m_flags, OLD))
	return -order;
    if (ison(msg1->m_flags, OLD) && isoff(msg2->m_flags, OLD))
	return order;
    if (ison(msg1->m_flags, UNREAD) && isoff(msg2->m_flags, UNREAD))
	return -order;
    if (isoff(msg1->m_flags, UNREAD) && ison(msg2->m_flags, UNREAD))
	return order;
    if (ison(msg1->m_flags,PRESERVE) && isoff(msg2->m_flags,PRESERVE))
	return -order;
    if (isoff(msg1->m_flags,PRESERVE) && ison(msg2->m_flags,PRESERVE))
	return order;

    return order;
}

author_cmp(msg1, msg2)
register struct msg *msg1, *msg2;
{
    char buf1[BUFSIZ], buf2[BUFSIZ];

    if (ison(glob_flags, WAS_INTR))
	longjmp(sortbuf, 1);
    if (msg1 < msg || msg2 < msg) {
	wprint("sort botch trying to sort %d and %d\n", msg1-msg, msg2-msg);
	return 0;
    }
    (void) reply_to(msg1 - msg, 0, buf1); /* "0" for "author only" */
    (void) reply_to(msg2 - msg, 0, buf2);
    Debug("author: msg %d: %s, msg %d: %s\n", msg1-msg, buf1, msg2-msg, buf2);
    return strcmp(buf1, buf2) * order;
}

/*
 * compare subject strings from two messages.
 * If Re is appended, so be it -- if user wants to ignore Re: use 'R' flag.
 */
subj_with_re(msg1, msg2)
register struct msg *msg1, *msg2;
{
    char buf1[256], buf2[256];

    if (ison(glob_flags, WAS_INTR))
	longjmp(sortbuf, 1);
    if (msg1 < msg || msg2 < msg) {
	wprint("sort botch trying to sort %d and %d\n", msg1-msg, msg2-msg);
	return 0;
    }
    (void) subject_to(msg1 - msg, buf1);
    (void) subject_to(msg2 - msg, buf2);
    Debug("subjects: (%d): \"%s\" (%d): \"%s\"\n", msg1-msg,buf1,msg2-msg,buf2);
    return strcmp(buf1, buf2) * order;
}

/*
 * Subject comparison ignoring Re:  subject_to() appends an Re: if there is
 * any subject whatsoever.  If so, skip 4 chars.  If not, set to ""
 */
subject_cmp(msg1, msg2)
register struct msg *msg1, *msg2;
{
    char buf1[256], buf2[256], *p1, *p2;

    if (ison(glob_flags, WAS_INTR))
	longjmp(sortbuf, 1);
    if (msg1 < msg || msg2 < msg) {
	wprint("sort botch trying to sort %d and %d\n", msg1-msg, msg2-msg);
	return 0;
    }
    if (p1 = subject_to(msg1 - msg, buf1))
	p1 += 4;
    else
	p1 = "";
    if (p2 = subject_to(msg2 - msg, buf2))
	p2 += 4;
    else
	p2 = "";
    Debug("subjects: (%d): \"%s\" (%d): \"%s\"\n", msg1-msg, p1, msg2-msg, p2);
    return strcmp(p1, p2) * order;
}

date_cmp(msg1, msg2)
register struct msg *msg1, *msg2;
{
    char buf1[11], buf2[11];

    if (ison(glob_flags, WAS_INTR))
	longjmp(sortbuf, 1);
    if (msg1 < msg || msg2 < msg) {
	wprint("sort botch trying to sort %d and %d\n", msg1-msg, msg2-msg);
	return 0;
    }
    (void) strcpy(buf1, msg_date(msg1-msg));
    (void) strcpy(buf2, msg_date(msg2-msg));
    Debug("dates: msg %d: %s, msg %d: %s\n", msg1-msg, buf1, msg2-msg, buf2);
    return strcmp(buf1, buf2) * order;
}
