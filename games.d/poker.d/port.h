/*
** Port for POKER.
*/

#ifdef INET
#  ifndef PORT
#    define	PORT		55142
#  endif
#  ifndef HOST
#    define	HOST		"ritcv"
#  endif
#else
#  ifndef PORT
#    define	PORT		"/usr/tmp/pokerd"
#  endif
#endif
