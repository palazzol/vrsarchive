/*
 * setup.c - set up all files for Phantasia
 */
#include "include.h"
#include <sys/types.h>
#include <sys/stat.h>
/**/
/************************************************************************
/
/ FUNCTION NAME: main()
/
/ FUNCTION: setup files for Phantasia 3.3.2
/
/ AUTHOR: E. A. Estes, 12/4/85
/
/ ARGUMENTS: none
/
/ RETURN VALUE: none
/
/ MODULES CALLED: time(), exit(), stat(), Error(), creat(), close(), fopen(), 
/	fgets(), floor(), srand(), umask(), random(), strcpy(), getuid(), 
/	unlink(), fwrite(), fclose(), sscanf(), printf(), strlen(), fprintf()
/
/ GLOBAL INPUTS: Peoplefile[], Curmonster, _iob[], Databuf[], *Monstfp, 
/	Lastdead[], Goldfile[], Voidfile[], Motdfile[], Messfile[], Scorefile[], 
/	Enemyfile[], Monstfile[], Enrgyvoid
/
/ GLOBAL OUTPUTS: Curmonster, Databuf[], *Monstfp, Enrgyvoid
/
/ DESCRIPTION: 
/
/	This program tries to verify the parameters specified in
/	the Makefile.
/
/	Create all necessary files.  Note that nothing needs to be
/	put in these files.
/	Also, the monster binary data base is created here.
/
/************************************************************************/
#ifdef __STDC__
#define STR(x)	#x
#define STRING(x)	STR(x)
#define FILENM(x)	STRING(DEST) "/" STRING(x)
#else
#define STRING(x)	"x
#define FILENM(x)	STRING(DEST)/x"
#endif

main()
{
FILE	*fp;			/* for opening files */
struct stat	fbuf;		/* for getting files statistics */
register char	**filename;	/* for pointing to file names */
register int	fd;		/* file descriptor */
static char *files[] =		/* all files to create */
	{
	Monstfile,
	Peoplefile,
	Messfile,
	Lastdead,
	Motdfile,
	Goldfile,
	Voidfile,
	Scorefile,
#ifdef ENEMY
	Enemyfile,
#endif
	(char *) NULL
	};

    srand((unsigned) time((long *) NULL));	/* prime random numbers */

    umask(077);				/* only owner can read/write created files */

    if ((getuid() != UIDA) && (getuid() != UIDB))
	fprintf(stderr, "Warning: UID (%d or %d) is not equal to current uid.\n", UIDA, UIDB);

    /* check Phantasia destination directory */
    if (stat(FILENM(.), &fbuf) < 0)
	/* not found */
	{
	Error("Cannot stat %s.\n", FILENM(.));
	exit(1);
	/*NOTREACHED*/
	}

    if ((fbuf.st_mode & S_IFDIR) == 0)
	/* not a directory */
	Error("%s is not a directory.\n", FILENM(.));
	/*NOTREACHED*/

    /* try to create data files */
    filename = &files[0];
    while (*filename != NULL)
	/* create each file */
	{
	if (stat(*filename, &fbuf) == 0)
	    /* file exists; remove it */
	    {
	    if (*filename == Peoplefile)
		/* do not reset character file if it already exists */
		{
		++filename;
		continue;
		}

	    if (unlink(*filename) < 0)
		Error("Cannot unlink %s.\n", *filename);
		/*NOTREACHED*/
	    }

	if ((fd = creat(*filename, 0600)) < 0)
	    Error("Cannot create %s.\n", *filename);
	    /*NOTREACHED*/

	close(fd);			/* close newly created file */

	++filename;			/* process next file */
	}

    /* put holy grail info into energy void file */
    Enrgyvoid.ev_active = TRUE;
    Enrgyvoid.ev_x = ROLL(-1.0e6, 2.0e6);
    Enrgyvoid.ev_y = ROLL(-1.0e6, 2.0e6);
    if ((fp = fopen(Voidfile, "w")) == NULL)
	Error("Cannot update %s.\n", Voidfile);
    else
	{
	fwrite(&Enrgyvoid, SZ_VOIDSTRUCT, 1, fp);
	fclose(fp);
	}

    /* create binary monster data base */
    if ((Monstfp = fopen(Monstfile, "w")) == NULL)
	Error("Cannot update %s.\n", Monstfile);
    else
	{
	if ((fp = fopen("monsters.asc", "r")) == NULL)
	    {
	    fclose(Monstfp);
	    Error("cannot open %s to create monster database.\n", "monsters.asc");
	    }
	else
	    {
	    Curmonster.m_o_strength =
	    Curmonster.m_o_speed =
	    Curmonster.m_maxspeed =
	    Curmonster.m_o_energy =
	    Curmonster.m_melee =
	    Curmonster.m_skirmish = 0.0;

	    while (fgets(Databuf, SZ_DATABUF, fp) != NULL)
		/* read in text file, convert to binary */
		{
		sscanf(&Databuf[24], "%F%F%F%F%F%d%d%F",
		    &Curmonster.m_strength, &Curmonster.m_brains,
		    &Curmonster.m_speed, &Curmonster.m_energy,
		    &Curmonster.m_experience, &Curmonster.m_treasuretype,
		    &Curmonster.m_type, &Curmonster.m_flock);
		Databuf[24] = '\0';
		strcpy(Curmonster.m_name, Databuf);
		fwrite((char *) &Curmonster, SZ_MONSTERSTRUCT, 1, Monstfp);
		}
	    fclose(fp);
	    fclose(Monstfp);
	    }
	}

    /* write to motd file */
    printf("One line 'motd' ? ");
    if (fgets(Databuf, SZ_DATABUF, stdin) == NULL)
	Databuf[0] = '\0';
    if ((fp = fopen(Motdfile, "w")) == NULL)
	Error("Cannot update %s.\n", Motdfile);
    else
	{
	fwrite(Databuf, sizeof(char), strlen(Databuf), fp);
	fclose(fp);
	}

    /* report compile-time options */
    printf("Compiled options:\n\n");
    printf("Phantasia destination directory:  %s\n", FILENM(.));
    printf("Wizard A:  %s   UID:  %d\n", WIZARDA, UIDA);
    printf("Wizard B:  %s   UID:  %d\n", WIZARDB, UIDB);

#ifdef OK_TO_PLAY
    printf("Restricted playing enabled.\n");
#else
    printf("Playing unrestricted.\n");
#endif

#ifdef ENEMY
    printf("Enemy list enabled.\n");
#else
    printf("Enemy list disabled.\n");
#endif

#ifdef SHELL
    printf("Shell escapes enabled.  Default shell:  %s\n", SHELL);
#else
    printf("Shell escapes disabled.\n");
#endif

#ifdef BSD41
    printf("Compiled for BSD 4.1\n");
#endif

#ifdef BSD42
    printf("Compiled for BSD 4.2\n");
#endif

#ifdef SYS3
    printf("Compiled for System III\n");
#endif

#ifdef SYS5
    printf("Compiled for System V\n");
#endif

    exit(0);
    /*NOTREACHED*/
}
/**/
/************************************************************************
/
/ FUNCTION NAME: Error()
/
/ FUNCTION: print an error message, and exit
/
/ AUTHOR: E. A. Estes, 12/4/85
/
/ ARGUMENTS:
/	char *str - format string for printf()
/	char *file - file which caused error
/
/ RETURN VALUE: none
/
/ MODULES CALLED: exit(), perror(), fprintf()
/
/ GLOBAL INPUTS: _iob[]
/
/ GLOBAL OUTPUTS: none
/
/ DESCRIPTION:
/	Print an error message, then exit.
/
/************************************************************************/

Error(str, file)
char	*str, *file;
{
    fprintf(stderr, "Error: ");
    fprintf(stderr, str, file);
    perror(file);
    exit(1);
    /*NOTREACHED*/
}
/**/
/************************************************************************
/
/ FUNCTION NAME: random()
/
/ FUNCTION: return a random number
/
/ AUTHOR: E. A. Estes, 2/7/86
/
/ ARGUMENTS: none
/
/ RETURN VALUE: none
/
/ MODULES CALLED: rand()
/
/ GLOBAL INPUTS: none
/
/ GLOBAL OUTPUTS: none
/
/ DESCRIPTION: 
/
/************************************************************************/

double
random()
{
    if (sizeof(int) != 2)
	return((double) (rand() & 0x7fff) / 32768.0);
    else
	return((double) rand() / 32768.0);
}
