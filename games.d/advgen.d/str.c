#include	"advgen.h"

char *
strsave(s)
    char	*s;
{
    char	*result;

    if ((result = malloc((unsigned) (strlen(s)+1))) == CNULL)
    {
	fatal("out of memory");
    }
    return	strcpy(result, s);
}
