/*
** vn news reader.
**
** envir_set.c - routine to obtain pertinent environment variable settings
**		and set up file / directory names
**
** see copyright disclaimer / history in vn.c source file
*/

#include <stdio.h>
#include <pwd.h>
#include <sys/param.h>
#include "tune.h"
#include "config.h"

extern char *Editor, *Ps1, *Printer;
extern char *Orgdir, *Savedir, *Ccfile;	/* path names */
extern char Cxitop[], Cxitor[], Cxrtoi[], Cxptoi[];
extern char *Home;

#ifdef SYSV
extern char *getcwd();
#define getwd(a) getcwd(a,sizeof(a))
#define	MAXPATHLEN 240
#else
extern char *getwd();
#endif

/*
	environment variable, original directory string setup.
*/

envir_set ()
{
 	char dbuf [MAXPATHLEN], *ccname, *keyxln;
	char *vn_env(), *getcwd(), *str_store();
	struct passwd *ptr, *getpwuid();

	vns_envir();

	Ps1 = vn_env("PS1",DEF_PS1);
	Editor = vn_env("EDITOR",DEF_ED);
	Printer = vn_env("PRINTER",DEF_PRINT);
	ccname = vn_env("CCFILE",DEF_CCFILE);
	keyxln = vn_env("VNKEY",DEF_KEYXLN);
	Savedir = vn_env("VNSAVE",NULL);

	/*
		set original directory strings.
	*/

	if ((ptr = getpwuid(getuid())) == NULL)
		printex("Cannot obtain /etc/passwd entry");
	Home = str_store(ptr->pw_dir);
	if ((Orgdir = getwd(dbuf)) == NULL)
		printex ("cannot stat pwd");
	Orgdir = str_store (Orgdir);
	if (Savedir == NULL)
		Savedir = Orgdir;
	if (*ccname != '/')
	{
		sprintf (dbuf, "%s/%s",Home,ccname);
		Ccfile = str_store (dbuf);
	}
	else
		Ccfile = str_store (ccname);
	sprintf (dbuf, "%s/%s%s",Home,".vn","XXXXXX");

	if (*keyxln != '/')
	{
		sprintf(dbuf, "%s/%s",Home,keyxln);
		set_kxln(dbuf);
	}
	else
		set_kxln(keyxln);
}

char *
vn_env(var,def)
char *var;
char *def;
{
	char pfx[RECLEN];
	char *res;
	char *getenv();

	if (var[0] != 'V' || var[1] != 'N')
	{
		sprintf(pfx,"VN%s",var);
		if ((res = getenv(pfx)) != NULL)
			return(res);
	}

	if ((res = getenv(var)) != NULL)
		return(res);

	return(def);
}

static
set_kxln(fname)
char *fname;
{
	FILE *fp;
	int i;
	char bufr[80];
	char in,out,*ptr;
	char *index(), xln_str();

	for (i=0; i < 128; ++i)
		Cxitop[i] = Cxitor[i] = Cxptoi[i] = Cxrtoi[i] = i;

	if ((fp = fopen(fname,"r")) != NULL)
	{
		while(fgets(bufr,79,fp) != NULL)
		{
			if (strncmp(bufr+1,"==",2) == 0)
				ptr = bufr+2;
			else
				ptr = index(bufr+1,'=');
			if (ptr == NULL)
				continue;
			*ptr = '\0';
			++ptr;
			in = xln_str(bufr+1);
			out = xln_str(ptr);
			switch(bufr[0])
			{
			case 'r':
			case 'R':
				Cxrtoi[out] = in;
				Cxitor[in] = out;
				break;
			case 'p':
			case 'P':
				Cxptoi[out] = in;
				Cxitop[in] = out;
			default:
				break;
			}
		}
		fclose(fp);
	}
}

static char
xln_str(s)
char *s;
{
	if (*s < '0' || *s > '9')
		return(*s & 0x7f);
	return((char)(atoi(s) & 0x7f));
}
