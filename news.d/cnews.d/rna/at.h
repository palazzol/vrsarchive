#define SECINWEEK	604800L
#define SECINDAY	 86400L
#define SECINHOUR	  3600L
#define SECINMIN	    60L
#define	DAYSTO1983	(10*365 + 3*366)
#define MAXTIME		0x7fffffffL

/*
 * frequencies
 */
#define HOURLY		1
#define DAILY		2
#define WEEKLY		3
#define MONTHLY		4
#define BOOT		5
#define BATCHTIME	6	/* not really a frequency - just looks like one */

/*
 * time types recognised
 */
#define DAYS		0	/* days only */
#define TIMES		1	/* days, times */
#define FULL		2	/* days, times, frequencies */
#define STIMES		3	/* days, times - be silent about errors */
