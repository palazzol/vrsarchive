/* Selfmade strings.h which is not included in MWC */
extern	char	*strcat();
extern	char	*strncat();

extern	int	strcmp();
extern	int	strncmp();

#ifndef GEMDOS
#define streq	!strcmp
#endif

extern	char	*strcpy();
extern	char	*strncpy();

extern	int	strlen();

extern char	*index();
extern char	*rindex();
