#include "cent.h"
#include "sys_dep.h"

#define INITOPT(x) in_map[x] = x
#define SETOPT(optstring,var) else if (!strcmp(s1,optstring)) in_map[*s3] = var

/* Initialize movement characters, possibly from options variable */
dooptions()
{
    char *optline,*tmp;
    char s1[100], s2[100], s3[100];

    INITOPT(LEFT);
    INITOPT(RIGHT);
    INITOPT(UPWARD);
    INITOPT(DOWN);
    INITOPT(FIRE);
    INITOPT(UPRIGHT);
    INITOPT(UPLEFT);
    INITOPT(DOWNRIGHT);
    INITOPT(DOWNLEFT);
    INITOPT(FASTLEFT);
    INITOPT(FASTRIGHT);
    INITOPT(PAUSEKEY);

    if ((optline = getenv("CENTOPTS")) == NULL)
	return;
    while (1)
    {
	sscanf(optline," %[^ =] %[=] %[^ ,]",s1,s2,s3);
	if (strcmp(s2,"=") || strlen(s3) != 1)
	    badopts();
	if (0)
	    ;
	SETOPT("fi",FIRE);
	SETOPT("up",UPWARD);
	SETOPT("do",DOWN);
	SETOPT("le",LEFT);
	SETOPT("ri",RIGHT);
	SETOPT("ul",UPLEFT);
	SETOPT("ur",UPRIGHT);
	SETOPT("dl",DOWNLEFT);
	SETOPT("dr",DOWNRIGHT);
	SETOPT("fl",FASTLEFT);
	SETOPT("fr",FASTRIGHT);
	SETOPT("pa",PAUSEKEY);
	else
	    badopts();
	if ((tmp = strchr(optline,',')) == NULL)
	    break;
	optline = tmp + 1;
    }
}

badopts()
{
    printf("Bad options specification in CENTOPTS\n");
    exit(1);
}
