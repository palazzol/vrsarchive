#ifdef VMS
/*
 * NOTE: VMS only
 */

#include "dbm.h"
#include <rms.h>
#include <stdio.h>

static struct FAB dbmfab;
static struct RAB dbmrab;
static struct XABKEY dbmxab;
static char *dbmfile;
static char reckey[24],recdat[512];

dbminit(file)
char *file;
{
	int e;

	dbmfile = file;

	dbmfab=cc$rms_fab;
	dbmrab=cc$rms_rab;
	dbmxab=cc$rms_xabkey;

	/* open the file */
	dbmfab.fab$b_fac = FAB$M_PUT + FAB$M_GET + FAB$M_DEL + FAB$M_UPD;
	dbmfab.fab$b_shr = FAB$M_SHRPUT + FAB$M_SHRGET + FAB$M_SHRDEL +
	FAB$M_SHRUPD;
	dbmfab.fab$l_fna = file;
	dbmfab.fab$b_fns = strlen(file);
	dbmfab.fab$l_xab = &dbmxab;
	if ((e=sys$open(&dbmfab)) != RMS$_KFF && e != RMS$_NORMAL) {
		fprintf(stderr,"dbminit 1 rms=%x ",e);
		return(-1);
	}

	/* set up for record I/O */
	dbmrab.rab$l_fab = &dbmfab;
	dbmrab.rab$b_krf = 0;
	if ((e=sys$connect(&dbmrab)) != RMS$_NORMAL) {
		fprintf(stderr,"dbminit 2 rms=%x ",e);
		return(-1);
	}

	return(0);
}

dbmclose()
{
	int e;

	if ((e=sys$close(&dbmfab)) != RMS$_NORMAL) {
		fprintf(stderr,"dbmclose rms=%x ",e);
		return(-1);
	}
	return(0);
}

datum fetch(key)
datum key;
{
	datum rec;
	int e;

	rec.dptr = (char *)0;

	dbmrab.rab$b_rac = RAB$C_KEY;
	dbmrab.rab$w_usz = sizeof(recdat);
	dbmrab.rab$l_ubf = recdat;
	dbmrab.rab$l_kbf = key.dptr;
	dbmrab.rab$b_ksz = key.dsize;
	dbmrab.rab$l_rop = RAB$M_NLK;
	if ((e=sys$get(&dbmrab)) != RMS$_NORMAL && e != RMS$_RNF) {
		fprintf(stderr,"fetch rms=%x ",e);
	}
	if (e == RMS$_NORMAL) {
		rec.dptr = dbmrab.rab$l_rbf + sizeof(reckey);
		rec.dsize = dbmrab.rab$w_rsz - sizeof(reckey);
	}
	return(rec);
}

dbmdelete(key)
datum key;
{
	int e;

	dbmrab.rab$b_rac = RAB$C_KEY;
	dbmrab.rab$l_kbf = key.dptr;
	dbmrab.rab$b_ksz = key.dsize;
	dbmrab.rab$l_rop = RAB$M_FDL;
	if ((e=sys$find(&dbmrab)) != RMS$_NORMAL && e != RMS$_RNF) {
		fprintf(stderr,"dbmdelete 1 rms=%x ",e);
	}
	if (e == RMS$_NORMAL) {
		if ((e=sys$delete(&dbmrab)) != RMS$_NORMAL) {
			fprintf(stderr,"dbmdelete 2 rms=%x ",e);
			return(-1);
		}
		return(0);
	} else
		return(-1);
}

store(key, dat)
datum key, dat;
{
	int e;

	dbmrab.rab$b_rac = RAB$C_KEY;
	binit(recdat,sizeof(reckey));
	bcopy(recdat,key.dptr,key.dsize);
	bcopy(recdat+sizeof(reckey),dat.dptr,dat.dsize);
	dbmrab.rab$l_kbf = recdat;
	dbmrab.rab$b_ksz = sizeof(reckey);
	dbmrab.rab$l_rbf = recdat;
	dbmrab.rab$w_rsz = sizeof(reckey) + dat.dsize;
	dbmrab.rab$l_rop = RAB$M_LOA + RAB$M_UIF;
	if ((e=sys$put(&dbmrab)) != RMS$_NORMAL) {
		fprintf(stderr,"store rms=%x ",e);
		return(-1);
	}
	return(0);
}

datum firstkey()
{
	datum key;
	int e;

	key.dptr = (char *)0;

	dbmrab.rab$b_rac = RAB$C_SEQ;
	if ((e=sys$rewind(&dbmrab)) != RMS$_NORMAL) {
		fprintf(stderr,"firstkey 1 rms=%x ",e);
	}
	if (e == RMS$_NORMAL) {
		dbmrab.rab$l_rop = RAB$M_NLK;
		dbmrab.rab$w_usz = sizeof(recdat);
		dbmrab.rab$l_ubf = recdat;
		if ((e=sys$get(&dbmrab)) != RMS$_NORMAL && e != RMS$_EOF) {
			fprintf(stderr,"firstkey 2 rms=%x ",e);
		}
		if (e == RMS$_NORMAL) {
			key.dptr = dbmrab.rab$l_rbf;
			key.dsize = sizeof(reckey);
		}
	}
	return(key);
}

datum nextkey(xkey)
datum xkey;
{
	datum key;
	int e;

	key.dptr = (char *)0;

	dbmrab.rab$b_rac = RAB$C_SEQ;
	dbmrab.rab$l_rop = RAB$M_NLK;
	dbmrab.rab$w_usz = sizeof(recdat);
	dbmrab.rab$l_ubf = recdat;
	if ((e=sys$get(&dbmrab)) != RMS$_NORMAL && e != RMS$_EOF) {
		fprintf(stderr,"nextkey rms=%x ",e);
	}
	if (e == RMS$_NORMAL) {
		key.dptr = dbmrab.rab$l_rbf;
		key.dsize = sizeof(reckey);
	}
	return(key);
}
#else /* !VMS */
/*
 * Incredibly slow Uglix dbm simulation.
 */

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>

#define MAXFILE 140
#define MAXLINE 1024

static char pagname[MAXFILE];
static FILE *db;
static int dbrdonly;

typedef struct {
	char *dptr;
	int dsize;
} datum;

dbminit(file)
char *file;
{
	char *strcat(), *strcpy();

	dbrdonly = 0;
	strcpy(pagname, file);
	strcat(pagname, ".pag");
	if ((db = fopen(pagname, "r+")) == NULL) {
		db = fopen(pagname, "r");
		dbrdonly = 1;
	}
	if (db == NULL) {
		fprintf(stderr, "cannot open database %s\n", file);
		return -1;
	}
	return 0;
}

static int getitem(), putitem();

datum
fetch(key)
datum key;
{
	datum item;

	rewind(db);
	while (getitem(&item, db) != EOF)		/* read key */
		if (strncmp(item.dptr, key.dptr, key.dsize) == 0)
			if (getitem(&item, db) == EOF)	/* read data */
				break;
			else
				return item;
	/* EOF */
	item.dptr = NULL;
	item.dsize = 0;
	return item;
}

delete(key)
datum key;
{
	datum item;
	FILE *temp;
	FILE *tmpfile();

	if (dbrdonly)
		return -1;
	temp = tmpfile();
	if (temp == NULL)
		return -1;
	/* copy from db to temp, omitting key & its data */
	rewind(db);
	while (getitem(&item, db) != EOF)
		if (strncmp(item.dptr, key.dptr, key.dsize) == 0) {
			if (getitem(&item, db) == EOF)	/* toss data too */
				return -1;
		} else
			if (putitem(&item, temp) == EOF)
				return -1;
	/* copy back from temp to db */
	rewind(temp);
	db = freopen(pagname, "w+", db);
	while (getitem(&item, temp) != EOF)
		if (putitem(&item, db) == EOF)
			return -1;
	return 0;
}

store(key, dat)
datum key, dat;
{
	if (dbrdonly)
		return -1;
#ifdef REALDBM			/* else, it's only for news */
	if (delete(key) == -1)
		return -1;
#endif
	if (putitem(&key, db) == EOF || putitem(&dat, db) == EOF)
		return -1;
	return 0;
}

datum
firstkey()
{
	datum trash;
	datum nextkey();

	rewind(db);
	return nextkey(trash);
}

datum
nextkey(key)			/* simplistic version, ignores key */
datum key;
{
	static datum dat;

	if (getitem(&dat, db) == EOF)
		dat.dptr = NULL;
	return dat;
}

static int
getitem(datump, fp)			/* points at static storage */
datum *datump;
FILE *fp;
{
	static char data[MAXLINE];

	if (fread((char *)&datump->dsize, sizeof datump->dsize, 1, fp) != 1)
		return EOF;
	datump->dptr = data;
	if (fread(data, datump->dsize, 1, fp) != 1)
		return EOF;
	return 0;
}

static int
putitem(datump, fp)
datum *datump;
FILE *fp;
{
	if (fwrite((char *)&datump->dsize, sizeof datump->dsize, 1, fp) != 1)
		return EOF;
	if (fwrite(datump->dptr, datump->dsize, 1, fp) != 1)
		return EOF;
	return 0;
}
#endif /* !VMS */
