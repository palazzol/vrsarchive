/*
 * long
 * maketime(argc, argv, type)
 *
 * A standard routine to convert a future time (in English) to seconds.
 * Arguments are order-independent (except for suffixes), and words
 * may be shortened to a non-ambiguous abbreviation.
 * As the time must be in the future, unspecified years, months and days default
 * to the "next" year, month or day if necessary; otherwise the current
 * month, day and hour are used.
 *
 * type is either TIMES in which days, times are recognised, or just DAYS.
 *
 * Tries hard to give meaningful messages, and make sure the user
 * gets the time she/he wanted!
 *
 * Return is in seconds or 0 if error.
 * Error messages to stderr.
 *
 * Michael Rourke (UNSW) Christmas 1982
 *
 * Syntax:
 *
 *	timespec ::= { time | day | month | year } .
 *	
 *	time ::= [ hour [ ":" min [ ":" second ] ] ] [ timemodifier ] .
 *	
 *	timemodifier ::= "am" | "pm" | "noon" | "midday" | "midnight" | "now" .
 *	
 *	day ::= ( dayofweek [ "week" ] ) | number .
 *	
 *	dayofweek ::= "sunday" | "monday" | "tuesday" | "wednesday" |
 *		      "thursday" | "friday" | "saturday" | "tomorrow" |
 *		      "today" .
 *	
 *	month ::= "january" | "february" | "march" | "april" | "may" | "june" |
 *		  "july" | "august" | "september" | "october" | "november" |
 *		  "december" .
 *	
 *	year ::= "19" number .
 *
 */

#include "defs.h"

#define	NOW 	-1

static timemod(), noonmid(), daymod(), weekday(), smonth();

static struct slist {
	char *s_name;
	int (*s_action)();
	char s_val;
	char s_type;
} slist[] = 
{
	{ "am", 		timemod, 	0, 	TIMES, 	 },
	{ "pm", 		timemod, 	12, 	TIMES, 	 },
	{ "noon", 		noonmid, 	12, 	TIMES, 	 },
	{ "midday", 	noonmid, 	12, 	TIMES, 	 },
	{ "midnight", 	noonmid, 	0, 	TIMES, 	 },
	{ "now", 		noonmid, 	NOW, 	TIMES, 	 },
	{ "week", 		daymod, 		0, 	DAYS, 	 },
	{ "sunday", 	weekday, 	0, 	DAYS, 	 },
	{ "monday", 	weekday, 	1, 	DAYS, 	 },
	{ "tuesday", 	weekday, 	2, 	DAYS, 	 },
	{ "wednesday", 	weekday, 	3, 	DAYS, 	 },
	{ "thursday", 	weekday, 	4, 	DAYS, 	 },
	{ "friday", 	weekday, 	5, 	DAYS, 	 },
	{ "saturday", 	weekday, 	6, 	DAYS, 	 },
	{ "tomorrow", 	weekday, 	7, 	DAYS, 	 },
	{ "today", 	weekday, 	8, 	DAYS, 	 },
	{ "january", 	smonth, 		0, 	DAYS, 	 },
	{ "february", 	smonth, 		1, 	DAYS, 	 },
	{ "march", 	smonth, 		2, 	DAYS, 	 },
	{ "april", 	smonth, 		3, 	DAYS, 	 },
	{ "may", 		smonth, 		4, 	DAYS, 	 },
	{ "june", 		smonth, 		5, 	DAYS, 	 },
	{ "july", 		smonth, 		6, 	DAYS, 	 },
	{ "august", 	smonth, 		7, 	DAYS, 	 },
	{ "september", 	smonth, 		8, 	DAYS, 	 },
	{ "october", 	smonth, 		9, 	DAYS, 	 },
	{ "november", 	smonth, 		10, 	DAYS, 	 },
	{ "december", 	smonth, 		11, 	DAYS, 	 },
	{ "", 		0, 		0, 	0, 	 }
};


static char daysinmonth[12] = 
{
	31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31
};


static int hour, minute, second, day, year, dayofweek, month;
static int settime, setday, setyear, setdayofweek, setmonth;
static int setweek, err, setmod;
static char *curarg;
static struct tm *tim;
static int gtype;		/* global `type' arg */
static short silent;

long
maketime(argc, argv, type)
int argc;
char **argv;
int type;
{
	struct tm *localtime();
	long time(), construct(), now, then;

	if (type == STIMES)
		type = TIMES, silent = 1;
	else
		silent = 0;
	gtype = type;
	now = time((long *) 0);
	tim = localtime(&now);

	/*
	 * set defaults 
	 */
	hour = tim->tm_hour;
	minute = tim->tm_min;
	second = tim->tm_sec;
	day = tim->tm_mday;
	year = tim->tm_year + 1900;
	dayofweek = tim->tm_wday;
	month = tim->tm_mon;

	settime = setday = setyear = setdayofweek = setmonth = 0;
	setweek = err = setmod = 0;

	while (argc--)
		timearg(curarg = *argv++);
	if (err)
		return (long) 0;

	checktime();
	if (err)
		return (long) 0;

	then = construct();
	/*
	if(now > then)
	{
		error("Time specified has passed.");
		return (long) 0;
	}
*/
	return then;
}


static
timearg(s)
char *s;
{
	lower(s);
	if (isdigit(*s))
		numbers(s);
	else
		words(s);
}


static
lower(s)
register char *s;
{
	while (*s) {
		*s = tolower(*s);
		s++;
	}
}


static
numbers(s)
register char *s;
{
	register int val;

	val = 0;
	while (isdigit(*s))
		val = val * 10 + *s++ - '0';
	if (val > 1900)
		if (setyear++)
			reperror("year");
		else
			year = val;
	else if (*s == '\0')
		if (setday++)
			reperror("day");
		else
			day = val;
	else if (settime++)
		reperror("time");
	else
	 {
		hour = val;
		if (*s == ':') {
			s++;
			val = 0;
			while (isdigit(*s))
				val = val * 10 + *s++ - '0';
			minute = val;
			if (*s == ':') {
				s++;
				val = 0;
				while (isdigit(*s))
					val = val * 10 + *s++ - '0';
				second = val;
			} else
				second = 0;
		} else
			minute = second = 0;
	}
	if (*s)
		words(curarg = s);
}


static
reperror(s)
char *s;
{
	error("Repeated %s argument: \"%s\"", s, curarg);
}


/* VARARGS1 */
static
error(s, a1, a2, a3, a4)
char *s;
int a1, a2, a3, a4;
{
	err++;
	if (silent)
		return;
	(void) fprintf(stderr, "Error in time specification: ");
	(void) fprintf(stderr, s, a1, a2, a3, a4);
	(void) fprintf(stderr, "\n");
}


static
words(s)
char *s;
{
	register struct slist *sp, *found;
	register int size;
	register char *wstart;

	sp = slist;
	wstart = s;
	size = 0;
	while (*s && !isdigit(*s))
		size++, s++;
	found = (struct slist *) 0;
	while (*(sp->s_name)) {
		if (sp->s_type <= gtype && CMPN(sp->s_name, wstart, size) ==
		    0)
			if (!found) {
				found = sp;
				if (strlen(sp->s_name) == size)
					break;	/* otherwise an abbreviation */
			}
			else
			 {
				error("Ambiguous abbreviation: \"%.*s\"", size,
				     wstart);
				return;
			}
		sp++;
	}
	if (found)
		(*(found->s_action))(found->s_val);
	else
		error("Unknown word: \"%.*s\"", size, wstart);
	if (*s)
		numbers(curarg = s);
}


static
timemod(val)
int val;
{
	if (!settime)
		error("Can only use \"am\" or \"pm\" after a time.");
	else if (setmod++)
		reperror("time modifier");
	else if (hour < 12)
		hour += val;
	else if (hour > 12)
		error("Can't use \"am\" or \"pm\" with 24 hour clock.");
	else if (val == 0) /* am && hour == 12 */
		hour = 0;	/* 12am correction */
}


static
noonmid(val)
int val;
{
	if (val < 0)	/* NOW */ {
		if (settime++)
			reperror("time");
		/* let defaults work */
	} else if (setmod++) /* noon, midnight */
		reperror("time modifier");
	else
	 {
		if (!settime)
			settime++;
		else if (hour != 12 || minute != 0 || second != 0)
			error("Illegal time: %02d:%02d:%02d %s", hour, minute,
			     second, curarg);
		hour = val;
		minute = second = 0;
	}
}


static
daymod()
{
	if (setweek++)
		reperror("\b");
	else if (!setdayofweek)
		error("Can only use \"week\" after a weekday name.");
	else
		dayofweek += 7;
}


static
weekday(val)
int val;
{
	if (setday++)
		reperror("day");
	else
	 {
		setdayofweek++;
		if (val < 7) {
			dayofweek = val - dayofweek;	/* now a displacement */
			if (dayofweek <= 0)
				dayofweek += 7;
		} else if (val == 7) /* tomorrow */
			dayofweek = 1;
		else	/* today */
			dayofweek = 0;
	}
}


static
smonth(val)
int val;
{
	if (setmonth++)
		reperror("day of month");
	else
		month = val;
}


static
checktime()
{
	register int dim;

	if (gtype == DAYS && settime)
		error("Times are not accepted.");
	if (year < 1983 || year > 2038)
		error("Year out of range.");
	if (hour > 23 || minute > 59 || second > 59)
		error("Illegal time: %02d:%02d:%02d", hour, minute, second);
	if (!setdayofweek) {
		dim = daysinmonth[month] + (month == 1 ? leapyear(year) : 0);
		if (day > dim)
			error("Month day out of range. (> %d)", dim);
	}
	if (setdayofweek && (setmonth || setyear))
		error("Can't specify a weekday as well as a month or year.");
}


static
leapyear(y)
int y;
{
	return ((y % 4) == 0 && (y % 100) != 0) || (y % 400 == 0);
}


static long 
construct()
{
	register int i, days;

	adjust();
	days = DAYSTO1983;
	for (i = 1983; i < year; i++)
		days += 365 + leapyear(i);
	for (i = 0; i < month; i++)
		days += daysinmonth[i] + (i == 1 ? leapyear(year) : 0);
	days += day - 1;	/* days since 1 Jan 1970 */
	if (setdayofweek)
		days += dayofweek;
	return days * SECINDAY + hour * SECINHOUR + minute * SECINMIN + second;
}


static
adjust()
{
	register int dim;

	/*
	 * make sure time defaults to the future
	 */
	if (setdayofweek || setyear || month > tim->tm_mon)
		return;
	if (month < tim->tm_mon) {
		year++;
		return;
	}
	/*
	 * month == tim->tm_mon
	 */
	if (day > tim->tm_mday)
		return;
	if (day < tim->tm_mday) {
		if (setmonth || ++month / 12)
			year++, month %= 12;
		return;
	}
	/*
	 * month == tim->tm_mon && day == tim->tm_mday
	 */
	if ((long)(hour*SECINHOUR + minute*SECINMIN + second) <
	    (long)(tim->tm_hour*SECINHOUR + tim->tm_min*SECINMIN + tim->tm_sec)) {
		dim = daysinmonth[month] + (month == 1? leapyear(month): 0);
		if (setday || ++day / dim) {
			if (setmonth || ++month / 12)
				year++, month %= 12;
			day %= dim;
		}
		return;
	}
}


