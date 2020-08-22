/* @(#)dates.c	1.1	(c) copyright 10/15/86 (Dan Heller) */

#include "mush.h"

char *day_names[] = {
    "Sun", "Mon", "Tues", "Wed", "Thurs", "Fri", "Sat"
};
char *month_names[] = {     /* imported in pick.c */
    "Jan", "Feb", "Mar", "Apr", "May", "Jun",
    "July", "Aug", "Sep", "Oct", "Nov", "Dec"
};

/* Time() returns a string according to criteria:
 *   if "now" is 0, then the current time is gotten and used.
 *       else, use the time described by now
 *   opts points to a string of args which is parsed until an unknown
 *       arg is found and opts will point to that upon return.
 *   valid args are T (time of day), D (day of week), M (month), Y (year),
 *       N (number of day in month -- couldn't think of a better letter).
 */
char *
Time(opts, now)
register char *opts;
long now;
{
    static char time_buf[30];
    struct tm 	  *T, *localtime();
    register char *p = time_buf;
    long	  x;

    if (!opts)
	return NULL;
    if (now)
	x = now;
    else
	(void) time(&x);
    T = localtime(&x);
    for (;; opts++) {
	switch(*opts) {
	    case 'T': (void) sprintf(p, "%d:%02d", (T->tm_hour) ?
		      ((T->tm_hour <= 12) ? T->tm_hour : T->tm_hour - 12) : 12,
			  T->tm_min);
	    when 'D': (void) strcpy(p, day_names[T->tm_wday]);
	    when 'M': (void) strcpy(p, month_names[T->tm_mon]);
	    when 'Y': (void) sprintf(p, "%d", T->tm_year);
	    when 'N': (void) sprintf(p, "%d", T->tm_mday);
	    otherwise: *--p = 0; return time_buf;
	}
	p += strlen(p);
	*p++ = ' ';
    }
}

/* find the date of a message and return a string of the same form
 * described by parse_date() below.
 */
char *
msg_date(n)
register int n;
{
    register char *p, *p2 = NULL;
    char line[256];

    /* not in use */
    /* try the easy way out first -- This is potentially a serious kludge
     * because not all message-id lines are right. -- most of the time,
     * this is correct.  it's not correct from messages from strange
     * mailers (non-sendmail) they do a different format in message-id.
    if ((p = header_field(n, "message-id")) && (p2 = index(p, '<'))) {
	p = p2+1;
	if (p2 = index(p, '.')) {
	    *p2 = 0;
	    return p;
	}
    }
     */

    /* else, get the "date" line, if that fails, get the date in "From" line */
    if ((p = header_field(n, "date")) && (p2 = parse_date(p)))
	return p2;

    (void) fseek(tmpf, msg[n].m_offset, L_SET);
    (void) fgets(line, BUFSIZ, tmpf);
    if (!(p = index(line, ' ')) || !(p2 = index(p+1, ' ')))
	return NULL;
    p = p2;

    if (!(p2 = parse_date(p)))
	print("Message %d has bad date: %s\n", n+1, p);
    return p2;
}

/* parse date and return a string that looks like
 *    "%2d%2d%2d%2d%2d", yr,mo,date,hrs,mins
 */
char *
parse_date(p)
register char *p;
{
    static char buf[11];
    int Month = 0, Day = 0, Year = 0, Hours = -1, Mins = -1;

    skipspaces(0);

    /* Possible combinations that we could have:
     *   day_number month_name year_number time timezone ...
     *   day_name month_name day_number time year_number
     *   day_name month_name day_number year_number time
     *   day_name day_number month_name year_number time
     *   day_number month_name year_number time
     *   day_number month_name year_number time-timezone (day)
     *                                       ^no colon separator
     *   day_name month_name day_number time timezone year_number
     *   day_number-month_name-year time
     *   day_name, day_number-month_name-year time
     */
    if (sscanf(p, "%d %s %2d %d:%d", &Day,buf,&Year,&Hours,&Mins) == 5
	||
	sscanf(p, "%*s %s %d %d:%d:%*d %d", buf,&Day,&Hours,&Mins,&Year) == 5
	||
	sscanf(p, "%*s %s %d %d %d:%d", buf,&Day,&Year,&Hours,&Mins) == 5
	||
	sscanf(p, "%*s %d %s %d %d:%d", &Day,buf,&Year,&Hours,&Mins) == 5
	||
	sscanf(p, "%d %s %d %d:%d", &Day,buf,&Year,&Hours,&Mins) == 5
	||
	sscanf(p, "%d %s %d %2d%2d", &Day,buf,&Year,&Hours,&Mins) == 5
	||
	sscanf(p, "%*s %s %d %d:%d:%*d %*s %d", 
					buf, &Day, &Hours, &Mins, &Year) == 5
	||
	sscanf(p,"%d-%[^-]-%d %d:%d", &Day, buf, &Year, &Hours, &Mins) == 5
	||
	sscanf(p,"%*s %d-%[^-]-%d %d:%d", &Day, buf, &Year, &Hours, &Mins) == 5) {
	if (Year > 1900)
	    Year -= 1900;
	if ((Month = month_to_n(buf)) == -1) {
	    print("bad month: %s\n", p);
	    return NULL;
	}
	return sprintf(buf, "%02d%02d%02d%02d%02d", Year,Month,Day,Hours,Mins);
    }
    print("Unknown date format: %s\n", p);
    return NULL;
}

/* pass a string in the form described above, put into string. "yr" is
 * not used, but is here anyway in case you want to modify it to use it.
 */
char *
date_to_string(Date)
char *Date;
{
    static char buf[30];
    int yr, mon, day, hr, mins;
    char a_or_p, *p = buf;

    (void) sscanf(Date, "%2d%2d%2d%2d%02d", &yr, &mon, &day, &hr, &mins);
    a_or_p = (hr < 12)? 'a': 'p';
    p += strlen(sprintf(p, "%s %2.d, ", month_names[mon-1], day));
    (void) sprintf(p, "%2.d:%02d%cm",
	  (hr)? (hr <= 12)? hr: hr - 12: 12, mins, a_or_p);
    return buf;
}

#define JAN	1
#define FEB	2
#define MAR	3
#define APR	4
#define MAY	5
#define JUN	6
#define JUL	7
#define AUG	8
#define SEP	9
#define OCT	10
#define NOV	11
#define DEC	12

/* stolen direct from ELM */
month_to_n(name)
register char *name;
{
    /** return the month number given the month name... **/

    register char ch;

    switch (lower(*name)) {
	case 'a' : if ((ch = lower(name[1])) == 'p')
		       return(APR);
		   else if (ch == 'u')
		       return(AUG);
		   else return(-1);	/* error! */
	case 'd' : return(DEC);
	case 'f' : return(FEB);
	case 'j' : if ((ch = lower(name[1])) == 'a')
		       return(JAN);
		   else if (ch == 'u') {
		     if ((ch = lower(name[2])) == 'n')
			 return(JUN);
		     else if (ch == 'l')
			 return(JUL);
		     else return(-1);		/* error! */
		   }
		   else return(-1);		/* error */
	case 'm' : if ((ch = lower(name[2])) == 'r')
		       return(MAR);
		   else if (ch == 'y')
		       return(MAY);
		   else return(-1);		/* error! */
	case 'n' : return(NOV);
	case 'o' : return(OCT);
	case 's' : return(SEP);
	default  : return(-1);
    }
}
