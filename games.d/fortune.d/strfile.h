/* @(#)strfile.h	1.2 (Berkeley) 5/14/81 */
# ifndef	__STRFILE__

# define	__STRFILE__

# define	MAXDELIMS	3

/*
 * bits for flag field
 */

# define	STR_RANDOM	0x1
# define	STR_ORDERED	0x2

typedef struct {		/* information table */
	unsigned long	str_numstr;		/* # of strings in the file */
	unsigned long	str_longlen;		/* length of longest string */
	unsigned long	str_shortlen;		/* length of shortest string */
	long		str_delims[MAXDELIMS];	/* delimiter markings */
	off_t		str_dpos[MAXDELIMS];	/* delimiter positions */
	short		str_flags;		/* bit field for flags */
} STRFILE;

#ifdef SYS5
# define REGCMP
#endif

# endif		__STRFILE__
