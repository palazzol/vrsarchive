/*
** Port for POKER.
*/

#ifdef INET
# define	PORT		55142
# define	HOST		"ritcv"
#else
#  ifndef PORT
#   define	PORT		"/usr/tmp/pokerd"
#  endif
#endif INET
