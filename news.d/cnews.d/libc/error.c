/*
 * error - print best error message possible and exit
 */

#include <stdio.h>

error(s1, s2)
char *s1;
char *s2;
{
	warning(s1, s2);
	exit(1);
}
